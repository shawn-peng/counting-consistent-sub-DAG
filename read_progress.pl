#!/usr/bin/perl

print "\033[H\033[J";
print "\033[r";

#$max_disp_line = `tput lines`;
$max_disp_line = 80;

$cur_level = 0;
$cur_line = 1;
@stack = qw();
#push (@stack, $cur_level);
#print "Initial stack:  @stack \n";
while (<>) {
	if (my ($line, $level, $mpnnum) = /( *\[(\d+)\] (\d+) MP nodes left.)/) {
		#print "$line $level $mpnnum\n";
		while ($level < $cur_level) {
			#go up line and clear current line
			if ($cur_level <= $max_disp_line) {
				print "\033[1A\033[2K";
			}
			#pop level stack
			$cur_level = pop (@stack);
		}

		push (@stack, $cur_level);
		$cur_level = $level;
		if ($cur_level <= $max_disp_line) {
			print "$line\n";
		}
	}
}

#$line = $1 if /(\[\d+\] \d+ MP nodes left.)/;
#print "$line\n";

#if (/\[(\d+)\] (\d+) MP nodes left./) {
#$level = $1, $mpnnum = $2;
#print "$level $mpnnum\n";
#}

