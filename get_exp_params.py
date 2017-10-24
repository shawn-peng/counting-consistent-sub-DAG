#!/bin/python

import sys

exp_name = sys.argv[1];

exp_dict = {
	"boundpivot"		: "--pivot bound",

};

print(exp_dict[exp_name]);

