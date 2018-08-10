#! /usr/bin/perl

use strict;

use WWW::Mechanize;
use Time::Local;
use JSON;

my $track4k_bin = "/usr/local/bin/track4k";
my $cropvid_bin = "/usr/local/bin/cropvid";
my $track4k_cfg = "/opt/opencast/wfexec/track4k.json";

my $track_in = $ARGV[0];
my $track_out = $ARGV[1];
my $location = $ARGV[2];

die "\nSyntax:\n $0 input-file output-file [location]\n\n" if (!defined($track_in) || !defined($track_out));

system("/usr/bin/logger","track4k in=$track_in out=$track_out location='$location'") == 0 or die "Cannot log params: $?";

my $json = "{}";

if (-e $track4k_cfg) {
  local $/; #Enable 'slurp' mode
  open my $fh, "<", $track4k_cfg;
  $json = <$fh>;
  close $fh;
  # print "JSON config: $json\n";
}

my $data = decode_json($json);

# Default resolution
(my $out_x, my $out_y) = (1920, 1080);

my $y_top;

# Set parameters from config
if (defined($data->{$location})) {
  my $output_size = $data->{$location}->{'output-size'};
  $y_top = $data->{$location}->{'y_top'};

  if (defined($output_size)) {
	if ($output_size eq "720p") {
		($out_x, $out_y) = (1280, 720);
	}
	if ($output_size eq "1620p") {
		($out_x, $out_y) = (2880, 1620);
	}
  }
  # print "Config for $location output-size $output_size out-x $out_x out-y $out_y\n";
}

# Cropping data
my $cropdata = "/tmp/track4k-$$-crop.txt";

# Redirect stdout and stderr
my $system_stdout = "/tmp/track4k-$$-stdout.log";
my $system_stderr = "/tmp/track4k-$$-stderr.log";

open(STDOUT, ">$system_stdout");
open(STDERR, ">$system_stderr");

# Run Track4K
if (defined($y_top)) {
  my @args = ($track4k_bin, $track_in, $cropdata, $out_x, $out_y, $y_top);
  system(@args) == 0 or die "executing @args failed: $?";
} else {
  my @args = ($track4k_bin, $track_in, $cropdata, $out_x, $out_y);
  system(@args) == 0 or die "executing @args failed: $?";
}

# Run cropvid
my @args = ($cropvid_bin, $track_in, $track_out, $cropdata);
system(@args) == 0 or die "executing @args failed: $?";

# Clean up
unlink($cropdata);
unlink($system_stdout);
unlink($system_stderr);

exit 0;
