#!/usr/bin/perl
#
# CGI script to create a table of values from RRD
use CGI qw(:all *table *Tr);
use RRDs;
use POSIX qw(strftime);
use POSIX qw(locale_h);

$ENV{'LANG'}='de_DE.UTF-8';
setlocale(LC_TIME, 'de_DE.UTF-8');

# path to database
$rrdb='../../emeocv/emeter.rrd';

$query=new CGI;
#$query=new CGI("type=consumweek");

print header(-charset=>"utf-8");

$type=$query->param('type');
if ($type !~ /(count|consum)(day|week|month|year)/) {
  print "Invalid type\n";
  exit;
}
$sensor=$1;
$range=$2;

#print "Sensor: $sensor Range: $range\n";

$timeformat="%a, %d. %b %Y";
$resolution=24*60*60;
if ($range eq 'day') {
  $resolution=60*60;
  $timeformat="%a, %d. %b %Y %H:%M";
} elsif ($range eq 'year') {
  $resolution=7*24*60*60;
  $timeformat="Wo %d. %b %Y";
}

if ($sensor eq 'count') {
  @indexes = (0);
  @headings = ('kWh');
  $title = "Zähler";
  $factor = 1.0;
  $cf = "LAST";
  $timeformat="%a, %d. %b %Y %H:%M";
} elsif ($sensor eq 'consum') {
  @indexes = (1);
  $title = "Leistung";
  if ($range eq 'day') {
    $factor = 1.0; # consumption in Watt
    @headings = ('W');
  } else {
    $factor = 24.0/1000.0; # consumption in kWh/day
    @headings = ('kWh/d');
  }
  $cf = "AVERAGE";
}


($start,$step,$names,$data) = getdata($rrdb, $cf, $resolution, $range);
# print result

# print heading
$style=<<END;
<!-- 
body {
  font: 10px/16px verdana, arial, helvetica, sans-serif;
  background-color: white;
  color: black;
}
th, td {
  border-width: 1px 1px 0px 0px;
  border-style: solid;
  border-color: #998DB3;
  text-align: right;
  width: 50px;
}
th {
  background-color: lightgreen;
  font-weight: normal;
}
thead {
  position: fixed;
  top: 0px;
  background-color: white;
  z-index: 1;
}
tbody {
  position: absolute;
  top: 30px;
  z-index: -1;
}
.mindata {
  color: blue;
}
.maxdata {
  color: red;
}
.time {
  color: black;
  text-align: left;
  width: 100px;
}
-->
END

print start_html(-title=>"Stromverbrauch - $title", 
  -style=>{-code=>$style},
  -meta=>{"viewport"=>"initial-scale=1, minimum-scale=0.75, width=device-width"}
);
print start_table();
print "\n";
print thead(Tr(th({-class=>'time'}, $title), th([@headings])));
print "\n";

# print data
$start -= 12*60*60 unless $timeformat =~ / %H:%M$/;
for $line (@$data) {
  $hastime=0;

  print start_Tr({-class=>'avrdata'});
  if (! $hastime) {
    print td({-class=>'time'}, strftime($timeformat, localtime($start)));
  }

  print formatline(@$line[@indexes]);
  print end_Tr;
  print "\n";

  $start += $step;
}
print end_table();
print end_html;

sub getdata {
  my ($db, $cf, $resolution, $range) = @_;

  my $sarg = "now -1 $range";
  if ($range eq 'month') {
    $sarg = "now - 30 days";
  }
  my ($start,$step,$names,$data) = RRDs::fetch($db, $cf, '-r', $resolution, '-s', $sarg, '-e', 'now');

  # check error
  my $err=RRDs::error;
  print "$err\n" if $err;
  exit if $err;
  return ($start,$step,$names,$data);
}

sub formatline {
  my @line = @_;
  my $str = '';
  for $val (@line) {
    if (defined $val) {
      $str .= td(sprintf "%5.1f", $val * $factor);
    } else {
      $str .= td;
    }
  }
  return $str;
}
