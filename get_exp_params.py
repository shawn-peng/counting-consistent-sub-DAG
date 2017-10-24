#!/bin/python

import sys

exp_name = sys.argv[1];

exp_dict = {
	"bruteforce"				: "--brute-force",

	"randpivot"					: "--pivot random",
	"randpivot_prune"			: "--pivot random prune",
	"randpivot_prune_hash"		: "--pivot random prune hash",

	"mpnpivot"					: "--pivot random_mpn",
	"mpnpivot_prune"			: "--pivot random_mpn prune",
	"mpnpivot_prune_hash"		: "--pivot random_mpn prune hash",

	"flowpivot"					: "--pivot flow",
	"flowpivot_prune"			: "--pivot flow prune",
	"flowpivot_prune_hash"		: "--pivot flow prune hash",

	"boundpivot"				: "--pivot bound",
	"boundpivot_prune"			: "--pivot bound prune",
	"boundpivot_prune_hash"		: "--pivot bound prune hash",

	"degreepivot"				: "--pivot degree",
	"degreepivot_prune"			: "--pivot degree prune",
	"degreepivot_prune_hash"	: "--pivot degree prune hash",

};

print(exp_dict[exp_name]);

