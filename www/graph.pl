#!/usr/bin/perl
#
# CGI script to create image using RRD graph 
use CGI qw(:all);
use RRDs;
use POSIX qw(locale_h);

$ENV{'LANG'}='de_DE.UTF-8';
setlocale(LC_ALL, 'de_DE.UTF-8');

# path to database
$rrd='../../emeocv/emeter.rrd';

# force to always create image from rrd 
#$force=1;

$query=new CGI;
#$query=new CGI("type=consumweek&size=small");

$size=$query->param('size');
$type=$query->param('type');
if ($size eq 'big') {
  $width=900;
  $height=700;
  $size="b";
} else {
  $width=500;
  $height=160;
  $size="s";
}
die "invalid type\n" unless $type =~ /(count|consum)(day|week|month|year)/; 
$ds=$1;
$range=$2;
$filename="/tmp/em${type}_${size}.png";

# create new image if existing file is older than rrd file
my $maxdiff = 10;
$maxdiff = 60 * 60 * 12 if $type eq 'tempyear';
if ((mtime($rrd) - mtime($filename) > $maxdiff) or $force) {
  $tmpfile="/tmp/emt${type}_${size}_$$.png";
  # call sub to create image according to type
  &$ds($range);
  # check error
  my $err=RRDs::error;
  die "$err\n" if $err;
  rename $tmpfile, $filename;
}

# feed image to stdout
open(IMG, $filename) or die "can't open $filename";
print header('image/png');
print <IMG>;
close IMG;

# end MAIN

# Return modification date of file
sub mtime {
  my $file=shift;
  my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$fsize,
       $atime,$mtime,$ctime,$blksize,$blocks)
           = stat($file);
  return $mtime;
}

sub count {
  my $range = shift;
  my @opts=(
    "-w", $width,
    "-h", $height,
    "-s", "now - 1 $range",
    "-e", "now",
    "-D",
    "-Y",
    "-X 0",
    "-A");
  RRDs::graph($tmpfile,
    @opts,
    "DEF:counter=$rrd:counter:LAST",
    "LINE2:counter#000000:Zähler",
    "VDEF:countlast=counter,LAST",
    "GPRINT:countlast:%.1lf kWh"
  );
}

sub consum {
  my $range = shift;
  my @opts=(
    "-w", $width,
    "-h", $height,
    "-D",
    "-Y",
    "-A",
    "-e", "now",
    "-s"
    );
  if ($range eq 'month') {
    push(@opts, "now - 30 days");
  } else {
    push(@opts, "now - 1 $range");
  }
  
  if ($range eq 'day') {
    RRDs::graph($tmpfile,
      @opts,
      "DEF:consum=$rrd:consum:AVERAGE",
      "LINE2:consum#00FF00:W",
      "CDEF:conskwh=consum,24,*,1000,/",
      "VDEF:conspd=conskwh,AVERAGE",
      'GPRINT:conspd:%.1lf kWh am Tag'
    );
  } else {
    RRDs::graph($tmpfile,
      @opts,
      "DEF:consum=$rrd:consum:AVERAGE",
      "CDEF:conspd=consum,24,*,1000,/",
      "LINE2:conspd#00FF00:kWh/d"
    );
  }
}
