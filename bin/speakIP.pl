#!/usr/bin/perl

# A little perl script to speak the wireless(wlan0) and ethernet(eth0) ip address using the espeak program
# pidora:
# wlan0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
#          inet 10.0.0.3  netmask 255.255.255.0  broadcast 10.0.0.255
#          inet6 fe80::76da:38ff:fe3a:fdbc  prefixlen 64  scopeid 0x20<link>

# raspbian:
# eth0      Link encap:Ethernet  HWaddr b8:27:eb:f9:c1:51
#            inet addr:192.168.1.90  Bcast:192.168.1.255  Mask:255.255.255.0
#            inet6 addr: fe80::ba27:ebff:fef9:c151/64 Scope:Link

while (1)
{

  # Check wireless...
  $IP_wireless = `ifconfig wlan0 | grep 'inet'`;
  $IP_ethernet = `ifconfig eth0 | grep 'inet'`;

  @ifaces = ();

  if ($IP_wireless =~ /inet (\d+.\d+.\d+.\d+)\s/)
  {
    # split address into its characters and join with spaces
    @characters = split(//,$1);
    $text = "wireless address " . join(" ", @characters);

    # map periods to the word dot
    $text =~ s/\./dot/g;
    @ifaces = ($text);
  }
  if ($IP_wireless =~ /inet addr:(\d+.\d+.\d+.\d+)\s/)
  {
    # split address into its characters and join with spaces
    @characters = split(//,$1);
    $text = "wireless address " . join(" ", @characters);

    # map periods to the word dot
    $text =~ s/\./dot/g;
    @ifaces = ($text);
  }

  if( $IP_ethernet  =~ /inet (\d+.\d+.\d+.\d+)\s/)
  {
    @characters = split(//,$1);
    $text = "ethernet address " . join(" ", @characters);

    $text =~ s/\./dot/g;
    push(@ifaces, $text);
  }
  if( $IP_ethernet  =~ /inet addr:(\d+.\d+.\d+.\d+)\s/)
  {
    @characters = split(//,$1);
    $text = "ethernet address " . join(" ", @characters);

    $text =~ s/\./dot/g;
    push(@ifaces, $text);
  }

  $size = @ifaces;
  if( $size <= 0 )
  {
    @ifaces = ("no IP address");
  }

  for my $i (0 .. $size)
  {
    print "$ifaces[$i]\n";
    system("espeak -s90 --stdout '$ifaces[$i]' | aplay");
  }

  # wait a minute before doing it again
  sleep 60;
}
