#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>


static int UUID_SIZE = 16;
char toHexChar(int b) {
    char c;
    if(b < 10)
        c = '0' + b;
    else
        c = 'A' + b - 10;
    return c;
}
int main(int argc, char **argv) {
    int b = (0xa4 & 0xf0) >> 4;
    printf("c0=%c, %d\n", toHexChar(b), b);
    b = (0xa4 & 0x0f);
    printf("c1=%c, %d\n", toHexChar(b), b);
    // The raw data from the manufacturer's data packate
    uint8_t data[] = {0x4c,0x0,
            0x2,0x15,
            0xa4,0x95,0xde,0xad,0xc5,0xb1,0x4b,0x44,0xb5,
            0x12,0x13,0x70,0xf0,0x2d,0x74,0xde,0x30,0x39,0xff,0xff,
            0xc5};
    // Get the manufacturer code from the first two octets
    int index = 0;
    int manufacturer = 256 * data[index++] + data[index++];

    // Get the first octet of the beacon code
    int code = 256 * data[index++] + data[index++];

    // Get the proximity uuid
    char uuid[UUID_SIZE+1];
    int n;
    for(n = 0; n < UUID_SIZE; n += 2, index ++) {
        int b0 = (data[index] & 0xf0) >> 4;
        int b1 = data[index] & 0x0f;
        char c0 = toHexChar(b0);
        char c1 = toHexChar(b1);
        uuid[n] = c0;
        uuid[n+1] = c1;
    }
    uuid[UUID_SIZE] = '\0';

    // Get the beacon major id
    int major = 256 * data[index++] + data[index++];
    // Get the beacon minor id
    int minor = 256 * data[index++] + data[index++];

    // Get the transmitted power, which is encoded as the 2's complement of the calibrated Tx Power
    int power = data[index] - 256;

    printf("uuid=%s, manufacturer=%d, code=%d, major=%d, minor=%d, power=%d\n", uuid, manufacturer, code, major, minor, power);
}