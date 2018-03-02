#!/usr/bin/perl

print "\033[H\033[J";
print "\033[r";

$max_disp_line = `tput lines`;
#$max_disp_line = 80;
#print "$max_disp_line";

$cur_level = 0;
$cur_line = 1;
@stack = qw();
#push (@stack, $cur_level);
#print "Initial stack:  @stack \n";
$exlines = 0;
while (<>) {
	if (my ($line, $level, $mpnnum) = /( *\[(\d+)\] (\d+) MP nodes left..*)/) {
		#print "$line $level $mpnnum\n";
		while ($level < $cur_level) {
			#go up line and clear current line
			#if ($cur_level < $max_disp_line) {
			if ($exlines == 0) {
				print "\033[1A\033[2K";
				$cur_line--;
			} else {
				$exlines--;
			}
			#pop level stack
			$cur_level = pop (@stack);
		}

		push (@stack, $cur_level);
		$cur_level = $level;
		if ($cur_line < $max_disp_line) {
			print "$line\n";
			$cur_line++;
		} else {
			$exlines++;
		}
	}
}

#$line = $1 if /(\[\d+\] \d+ MP nodes left.)/;
#print "$line\n";

#if (/\[(\d+)\] (\d+) MP nodes left./) {
#$level = $1, $mpnnum = $2;
#print "$level $mpnnum\n";
#}

