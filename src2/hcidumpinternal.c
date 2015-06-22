#include "hcidumpinternal.h"

/*
 *  Code from the bluez tools/hcidump.c
 *  Copyright 2015 Red Hat
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2000-2002  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2003-2011  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <config.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <ctype.h>

#include "parser/parser.h"
#include "parser/sdp.h"

#include "lib/hci.h"
#include "lib/hci_lib.h"

#define SNAP_LEN	HCI_MAX_FRAME_SIZE
// The size of the uuid in the manufacturer data
#define UUID_SIZE 16
// The minimum size of manufacturer data we are interested in. This consists of:
// manufacturer(2), code(2), uuid(16), major(2), minor(2), calibrated power(1)
#define MIN_MANUFACTURER_DATA_SIZE (2+2+UUID_SIZE+2+2+1)

/* Modes */
enum {
    PARSE,
    READ,
    WRITE,
    PPPDUMP,
    AUDIO
};

// A stop flag
bool stop_scan_frames = false;

/* Default options */
static int  snap_len = SNAP_LEN;

struct hcidump_hdr {
    uint16_t	len;
    uint8_t		in;
    uint8_t		pad;
    uint32_t	ts_sec;
    uint32_t	ts_usec;
} __attribute__ ((packed));
#define HCIDUMP_HDR_SIZE (sizeof(struct hcidump_hdr))

struct btsnoop_hdr {
    uint8_t		id[8];		/* Identification Pattern */
    uint32_t	version;	/* Version Number = 1 */
    uint32_t	type;		/* Datalink Type */
} __attribute__ ((packed));
#define BTSNOOP_HDR_SIZE (sizeof(struct btsnoop_hdr))

struct btsnoop_pkt {
    uint32_t	size;		/* Original Length */
    uint32_t	len;		/* Included Length */
    uint32_t	flags;		/* Packet Flags */
    uint32_t	drops;		/* Cumulative Drops */
    uint64_t	ts;		/* Timestamp microseconds */
    uint8_t		data[0];	/* Packet Data */
} __attribute__ ((packed));
#define BTSNOOP_PKT_SIZE (sizeof(struct btsnoop_pkt))

static uint8_t btsnoop_id[] = { 0x62, 0x74, 0x73, 0x6e, 0x6f, 0x6f, 0x70, 0x00 };

static uint32_t btsnoop_version = 0;
static uint32_t btsnoop_type = 0;

struct pktlog_hdr {
    uint32_t	len;
    uint64_t	ts;
    uint8_t		type;
} __attribute__ ((packed));
#define PKTLOG_HDR_SIZE (sizeof(struct pktlog_hdr))

static int open_socket(int dev)
{
    struct sockaddr_hci addr;
    struct hci_filter flt;
    int sk, opt;

    /* Create HCI socket */
    sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0) {
        perror("Can't create raw socket");
        return -1;
    }

    opt = 1;
    if (setsockopt(sk, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0) {
        perror("Can't enable data direction info");
        return -1;
    }

    opt = 1;
    if (setsockopt(sk, SOL_HCI, HCI_TIME_STAMP, &opt, sizeof(opt)) < 0) {
        perror("Can't enable time stamp");
        return -1;
    }

    /* Setup filter */
    hci_filter_clear(&flt);
    hci_filter_all_ptypes(&flt);
    hci_filter_all_events(&flt);
    if (setsockopt(sk, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        perror("Can't set filter");
        return -1;
    }

    /* Bind socket to the HCI device */
    memset(&addr, 0, sizeof(addr));
    addr.hci_family = AF_BLUETOOTH;
    addr.hci_dev = dev;
    if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Can't attach to device hci%d. %s(%d)\n",
                dev, strerror(errno), errno);
        return -1;
    }

    return sk;
}

#define EVENT_NUM 77
static char *event_str[EVENT_NUM + 1] = {
        "Unknown",
        "Inquiry Complete",
        "Inquiry Result",
        "Connect Complete",
        "Connect Request",
        "Disconn Complete",
        "Auth Complete",
        "Remote Name Req Complete",
        "Encrypt Change",
        "Change Connection Link Key Complete",
        "Master Link Key Complete",
        "Read Remote Supported Features",
        "Read Remote Ver Info Complete",
        "QoS Setup Complete",
        "Command Complete",
        "Command Status",
        "Hardware Error",
        "Flush Occurred",
        "Role Change",
        "Number of Completed Packets",
        "Mode Change",
        "Return Link Keys",
        "PIN Code Request",
        "Link Key Request",
        "Link Key Notification",
        "Loopback Command",
        "Data Buffer Overflow",
        "Max Slots Change",
        "Read Clock Offset Complete",
        "Connection Packet Type Changed",
        "QoS Violation",
        "Page Scan Mode Change",
        "Page Scan Repetition Mode Change",
        "Flow Specification Complete",
        "Inquiry Result with RSSI",
        "Read Remote Extended Features",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Synchronous Connect Complete",
        "Synchronous Connect Changed",
        "Sniff Subrate",
        "Extended Inquiry Result",
        "Encryption Key Refresh Complete",
        "IO Capability Request",
        "IO Capability Response",
        "User Confirmation Request",
        "User Passkey Request",
        "Remote OOB Data Request",
        "Simple Pairing Complete",
        "Unknown",
        "Link Supervision Timeout Change",
        "Enhanced Flush Complete",
        "Unknown",
        "User Passkey Notification",
        "Keypress Notification",
        "Remote Host Supported Features Notification",
        "LE Meta Event",
        "Unknown",
        "Physical Link Complete",
        "Channel Selected",
        "Disconnection Physical Link Complete",
        "Physical Link Loss Early Warning",
        "Physical Link Recovery",
        "Logical Link Complete",
        "Disconnection Logical Link Complete",
        "Flow Spec Modify Complete",
        "Number Of Completed Data Blocks",
        "AMP Start Test",
        "AMP Test End",
        "AMP Receiver Report",
        "Short Range Mode Change Complete",
        "AMP Status Change",
};

#define LE_EV_NUM 5
static char *ev_le_meta_str[LE_EV_NUM + 1] = {
        "Unknown",
        "LE Connection Complete",
        "LE Advertising Report",
        "LE Connection Update Complete",
        "LE Read Remote Used Features Complete",
        "LE Long Term Key Request",
};

static const char *bdaddrtype2str(uint8_t type)
{
    switch (type) {
        case 0x00:
            return "Public";
        case 0x01:
            return "Random";
        default:
            return "Reserved";
    }
}

static const char *evttype2str(uint8_t type)
{
    switch (type) {
        case 0x00:
            return "ADV_IND - Connectable undirected advertising";
        case 0x01:
            return "ADV_DIRECT_IND - Connectable directed advertising";
        case 0x02:
            return "ADV_SCAN_IND - Scannable undirected advertising";
        case 0x03:
            return "ADV_NONCONN_IND - Non connectable undirected advertising";
        case 0x04:
            return "SCAN_RSP - Scan Response";
        default:
            return "Reserved";
    }
}

char toHexChar(int b) {
    char c = '0';
    if(b < 10)
        c = '0' + b;
    else
        c = 'A' + b - 10;
    return c;
}
static inline void print_bytes(uint8_t *data, u_int8_t len) {
    int n;
    printf("{");
    for(n = 0; n < len; n ++) {
        printf("0x%x,", data[n]);
    }
    printf("};\n");
}
static inline void hex_debug(int level, struct frame *frm)
{
    unsigned char *buf = frm->ptr;
    register int i, n;
    int num = frm->len;

    for (i = 0, n = 1; i < num; i++, n++) {
        if (n == 1)
            p_indent(level, frm);
        printf("%2.2X ", buf[i]);
        if (n == DUMP_WIDTH) {
            printf("\n");
            n = 0;
        }
    }
    if (i && n != 1)
        printf("\n");
}

/**
* Parse the AD Structure values found in the AD payload
*/
static inline void ext_inquiry_data_dump(int level, struct frame *frm, uint8_t *data, beacon_info *info) {
    uint8_t len = data[0];
    uint8_t type;
    char *str;
    int i;

    if (len == 0)
        return;

    type = data[1];
    data += 2;
    len -= 1;

    switch (type) {
        case 0x01:
#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("Flags:");
            for (i = 0; i < len; i++)
                printf(" 0x%2.2x", data[i]);
            printf("\n");
#endif
            break;

        case 0x02:
        case 0x03:
            // incomplete/complete List of 16-bit Service Class UUIDs
        case 0x04:
        case 0x05:
            // incomplete/complete List of 32-bit Service Class UUIDs
        case 0x06:
        case 0x07:
#ifdef PRINT_DEBUG
            // incomplete/complete List of 128-bit Service Class UUIDs
            p_indent(level, frm);
            printf("%s service classes:", type == 0x06 ? "Incomplete" : "Complete");
            for (i = 0; i < len / 2; i++)
                printf(" 0x%4.4x", get_le16(data + i * 2));
            printf("\n");
#endif
            break;

        case 0x08:
        case 0x09:
#ifdef PRINT_DEBUG
            str = malloc(len + 1);
            if (str) {
                snprintf(str, len + 1, "%s", (char *) data);
                for (i = 0; i < len; i++)
                    if (!isprint(str[i]))
                        str[i] = '.';
                p_indent(level, frm);
                printf("%s local name: \'%s\'",
                        type == 0x08 ? "Shortened" : "Complete", str);
                for (i = 0; i < len; i++)
                    printf(" 0x%2.2x", data[i]);
                printf("\n");
                free(str);
            }
#endif
            break;

        case 0x0a:
            info->power = *((uint8_t *) data);
#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("TX power level: %d\n", info->power);
#endif

        case 0x12:
            //  Slave Connection Interval Range sent by Gimbals
#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("Slave Connection Interval Range %d bytes data\n", type, len);
#endif
            break;
        case 0x16:
            printf("ServiceData16(%x %x), len=%d", data[0], data[1], len);
            for (i = 2; i < len; i++)
                printf(" 0x%2.2x", data[i]);
            printf("\n");
            break;
        case 0xff:

#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("ManufacturerData(%d bytes), valid=%d:", len, len >= MIN_MANUFACTURER_DATA_SIZE);
            print_bytes(data, len);
            hex_debug(level, frm);
#endif
            // Skip any event that has less than the minimum data size for a beacon event
            if(len < MIN_MANUFACTURER_DATA_SIZE)
                return;

            info->time = frm->ts.tv_sec;
            info->time *= 1000;
            info->time += frm->ts.tv_usec/1000;
            // Get the manufacturer code from the first two octets
            int index = 0;
            info->manufacturer = 256 * data[index++] + data[index++];

            // Get the first octet of the beacon code
            info->code = 256 * data[index++] + data[index++];

            // Get the proximity uuid
            int n;
            for(n = 0; n < 2*UUID_SIZE; n += 2, index ++) {
                int b0 = (data[index] & 0xf0) >> 4;
                int b1 = data[index] & 0x0f;
                char c0 = toHexChar(b0);
                char c1 = toHexChar(b1);
                info->uuid[n] = c0;
                info->uuid[n+1] = c1;
            }
            // null terminate the 2*UUID_SIZE bytes that make up the uuid string
            info->uuid[2*UUID_SIZE] = '\0';
#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("UUID(%d):%s\n", strlen(info->uuid), info->uuid);
#endif
            // Get the beacon major id
            int m0 = data[index++];
            int m1 = data[index++];
            info->major = 256 * m0 + m1;
            // Get the beacon minor id
            m0 = data[index++];
            m1 = data[index++];
            info->minor = 256 * m0 + m1;
#ifdef PRINT_DEBUG
            p_indent(level, frm);
            printf("Major:%d, Minor:%d\n", info->major, info->minor);
#endif

            // Get the transmitted power, which is encoded as the 2's complement of the calibrated Tx Power
            info->calibrated_power = data[index] - 256;
            break;

        default:
            p_indent(level, frm);
            printf("Unknown type 0x%02x with %d bytes data\n", type, len);
            break;
    }
}

static inline void evt_le_advertising_report_dump(int level, struct frame *frm, beacon_info *binfo)
{
    uint8_t num_reports = get_u8(frm);
    const uint8_t RSSI_SIZE = 1;

    while (num_reports--) {
        char addr[18];
        le_advertising_info *info = frm->ptr;
        int offset = 0;

        p_ba2str(&info->bdaddr, addr);

#ifdef PRINT_DEBUG
        p_indent(level, frm);
        printf("%s (%d)\n", evttype2str(info->evt_type), info->evt_type);

        p_indent(level, frm);
        printf("bdaddr %s (%s)\n", addr, bdaddrtype2str(info->bdaddr_type));
#endif
        while (offset < info->length) {
            int eir_data_len = info->data[offset];

            ext_inquiry_data_dump(level, frm, &info->data[offset], binfo);

            offset += eir_data_len + 1;
        }

        frm->ptr += LE_ADVERTISING_INFO_SIZE + info->length;
        frm->len -= LE_ADVERTISING_INFO_SIZE + info->length;

        binfo->rssi = ((int8_t *) frm->ptr)[frm->len - 1];
#ifdef PRINT_DEBUG
        p_indent(level, frm);
        printf("RSSI: %d\n", binfo->rssi);
#endif
        frm->ptr += RSSI_SIZE;
        frm->len -= RSSI_SIZE;
    }
}

static inline void le_meta_ev_dump(int level, struct frame *frm, beacon_info *info)
{
    evt_le_meta_event *mevt = frm->ptr;
    uint8_t subevent;

    subevent = mevt->subevent;

    frm->ptr += EVT_LE_META_EVENT_SIZE;
    frm->len -= EVT_LE_META_EVENT_SIZE;

#ifdef PRINT_DEBUG
    p_indent(level, frm);
    printf("%s\n", ev_le_meta_str[subevent]);
#endif

    switch (mevt->subevent) {
        case EVT_LE_CONN_COMPLETE:
            //evt_le_conn_complete_dump(level + 1, frm);
            printf("Skipping EVT_LE_CONN_COMPLETE\n");
            break;
        case EVT_LE_ADVERTISING_REPORT:
            evt_le_advertising_report_dump(level + 1, frm, info);
            break;
        case EVT_LE_CONN_UPDATE_COMPLETE:
            //evt_le_conn_update_complete_dump(level + 1, frm);
            printf("Skipping EVT_LE_CONN_UPDATE_COMPLETE\n");
            break;
        case EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
            //evt_le_read_remote_used_features_complete_dump(level + 1, frm);
            printf("Skipping EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE\n");
            break;
        default:
            hex_debug(level, frm);
            break;
    }
}


static inline void event_dump(int level, struct frame *frm, beacon_info *info)
{
    hci_event_hdr *hdr = frm->ptr;
    uint8_t event = hdr->evt;

    if (event <= EVENT_NUM) {
#ifdef PRINT_DEBUG
        p_indent(level, frm);
        printf("HCI Event: %s (0x%2.2x) plen %d\n", event_str[hdr->evt], hdr->evt, hdr->plen);
#endif
    } else if (hdr->evt == EVT_TESTING) {
        p_indent(level, frm);
        printf("HCI Event: Testing (0x%2.2x) plen %d\n", hdr->evt, hdr->plen);
    } else {
        p_indent(level, frm);
        printf("HCI Event: code 0x%2.2x plen %d\n", hdr->evt, hdr->plen);
    }

    frm->ptr += HCI_EVENT_HDR_SIZE;
    frm->len -= HCI_EVENT_HDR_SIZE;

    if (event == EVT_CMD_COMPLETE) {
        evt_cmd_complete *cc = frm->ptr;
        if (cc->opcode == cmd_opcode_pack(OGF_INFO_PARAM, OCF_READ_LOCAL_VERSION)) {
            read_local_version_rp *rp = frm->ptr + EVT_CMD_COMPLETE_SIZE;
        }
    }

    switch (event) {
        case EVT_LOOPBACK_COMMAND:
            printf("Skipping EVT_LOOPBACK_COMMAND\n");
            break;
        case EVT_CMD_COMPLETE:
            printf("Skipping EVT_CMD_COMPLETE\n");
            break;
        case EVT_LE_META_EVENT:
            le_meta_ev_dump(level + 1, frm, info);
            break;

        default:
            printf("Skipping event=%d\n", event);
            raw_dump(level+1, frm);
            break;
    }
}

static void do_parse(struct frame *frm, beacon_info *info) {
    uint8_t type = *(uint8_t *)frm->ptr;

    frm->ptr++; frm->len--;
    switch (type) {
        case HCI_EVENT_PKT:
            event_dump(0, frm, info);
            break;

        default:
            p_indent(0, frm);
            printf("Unknown: type 0x%2.2x len %d\n", type, frm->len);
            hex_debug(0, frm);
            break;
    }

}

/*
    This is the process_frames function from hcidump.c with the addition of the beacon_event callback
 */
int process_frames(int dev, int sock, int fd, unsigned long flags, beacon_event callback)
{
    struct cmsghdr *cmsg;
    struct msghdr msg;
    struct iovec  iv;
    struct hcidump_hdr *dh;
    struct btsnoop_pkt *dp;
    struct frame frm;
    struct pollfd fds[2];
    int nfds = 0;
    char *buf, *ctrl;
    int len, hdr_size = HCIDUMP_HDR_SIZE;

    if (sock < 0)
        return -1;

    if (snap_len < SNAP_LEN)
        snap_len = SNAP_LEN;

    if (flags & DUMP_BTSNOOP)
        hdr_size = BTSNOOP_PKT_SIZE;

    buf = malloc(snap_len + hdr_size);
    if (!buf) {
        perror("Can't allocate data buffer");
        return -1;
    }

    frm.data = buf + hdr_size;

    ctrl = malloc(100);
    if (!ctrl) {
        free(buf);
        perror("Can't allocate control buffer");
        return -1;
    }

    if (dev == HCI_DEV_NONE)
        printf("system: ");
    else
        printf("device: hci%d ", dev);

    printf("snap_len: %d filter: 0x%lx\n", snap_len, parser.filter);

    memset(&msg, 0, sizeof(msg));

    fds[nfds].fd = sock;
    fds[nfds].events = POLLIN;
    fds[nfds].revents = 0;
    nfds++;

    long frameNo = 0;
    bool stopped = false;
    while (!stopped) {
        int i, n = poll(fds, nfds, 5000);

        if (n <= 0) {
            if(stop_scan_frames)
                break;
            continue;
        }

        for (i = 0; i < nfds; i++) {
            if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                if (fds[i].fd == sock)
                    printf("device: disconnected\n");
                else
                    printf("client: disconnect\n");
                return 0;
            }
        }

        iv.iov_base = frm.data;
        iv.iov_len  = snap_len;

        msg.msg_iov = &iv;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl;
        msg.msg_controllen = 100;

        len = recvmsg(sock, &msg, MSG_DONTWAIT);
        if (len < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            perror("Receive failed");
            return -1;
        }

        /* Process control message */
        frm.data_len = len;
        frm.dev_id = dev;
        frm.in = 0;

        cmsg = CMSG_FIRSTHDR(&msg);
        beacon_info info;
        while (cmsg) {
            int dir;
            switch (cmsg->cmsg_type) {
                case HCI_CMSG_DIR:
                    memcpy(&dir, CMSG_DATA(cmsg), sizeof(int));
                    frm.in = (uint8_t) dir;
                    break;
                case HCI_CMSG_TSTAMP:
                    memcpy(&frm.ts, CMSG_DATA(cmsg), sizeof(struct timeval));
                    break;
            }
            cmsg = CMSG_NXTHDR(&msg, cmsg);
        }

        frm.ptr = frm.data;
        frm.len = frm.data_len;

        /* Parse and print */
        memset(&info, 0, sizeof(info));
        frameNo ++;
#ifdef PRINT_DEBUG
        printf("Begin do_parse(ts=%ld.%ld)#%ld\n", frm.ts.tv_sec, frm.ts.tv_usec, frameNo);
#endif
        int64_t time = -1;
        do_parse(&frm, &info);
        time = info.time;
        if(time > 0)
            stopped = callback(&info);
#ifdef PRINT_DEBUG
        printf("End do_parse(info.time=%lld)\n", time);
#endif

    }

    return 0;
}

int scan_frames(int device, beacon_event callback) {
    unsigned long flags = 0;

    flags |= DUMP_TSTAMP;
    flags |= DUMP_EXT;
    flags |= DUMP_VERBOSE;
    int socketfd = open_socket(device);
    printf("Scanning hci%d, socket=%d\n", device, socketfd);
    return process_frames(device, socketfd, -1, flags, callback);
}
