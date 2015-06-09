#!/bin/perl

print "pnm2bin.pl picture.pnm picture.bin\n";
print "converts a picture in the format 240x320 PNM to the RGB-565 format necessary for Fernvale\n";

my $infile=$ARGV[0] || "picture.pnm";
my $outfile=$ARGV[1] || "picture.bin";

open IN,"<$infile";
my $header=<IN>; $header=~s/\s*$//;
if($header=~m/^P[36]/)
{
  print "$header PNM Format detected. Good. Converting ...\n";
  my $size=<IN>;
  while($size=~m/^#/)
  {
    $size=<IN>;
  }
  if($size=~m/^(\d+) (\d+)/)
  {
    my $xsize=$1; my $ysize=$2;
    print "Format: $1x$2\n";
    my $value=<IN>;
    open OUT,">$outfile";

    foreach(0 .. ($ysize-1))
    {
      foreach(0 .. ($xsize-1))
      {
        my $r=($header eq "P6")?unpack("C",getc(IN)):<IN>; $r>>=3;
        my $g=($header eq "P6")?unpack("C",getc(IN)):<IN>; $g>>=2;
        my $b=($header eq "P6")?unpack("C",getc(IN)):<IN>; $b>>=3;

        my $p1=(($g&7)<<5) | ($b);
        my $p2=($r<<3) | ($g>>3);
        print OUT pack("C",$p1);
        print OUT pack("C",$p2);
      }
    }
    close OUT;
  }
}
close IN;
