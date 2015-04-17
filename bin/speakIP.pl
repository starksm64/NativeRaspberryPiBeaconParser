#!/usr/bin/perl

# A little perl script to speak the wireless(wlan0) and ethernet(eth0) ip address using the espeak program
while (1)
{

  # Check wireless...
  $IP_wireless = `ifconfig wlan0 | grep 'inet'`;
  $IP_ethernet = `ifconfig eth0 | grep 'inet'`;

  if ($IP_wireless =~ /inet (.*?) /)
  {
    # split address into its characters and join with spaces
    @characters = split(//,$1);
    $text = "wireless address " . join(" ", @characters);

    # map periods to the word dot
    $text =~ s/\./dot/g;
    @ifaces = ($text);
  }

  if( $IP_ethernet  =~ /inet (.*?) /)
  {
    @characters = split(//,$1);
    $text = "ethernet address " . join(" ", @characters);

    $text =~ s/\./dot/g;
    push(@ifaces, $text);
  }

  if( $#ifaces <= 0 )
  {
    @ifaces = ("no IP address");
  }

  for my $i (0 .. $#ifaces)
  {
    print "$ifaces[$i]\n";
    system("espeak -s90 --stdout '$ifaces[$i]' | aplay");
  }

  # wait a minute before doing it again
  sleep 60;
}
