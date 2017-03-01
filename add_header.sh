#!/bin/bash

sed -e "1s/^.*$/parent\tchild\n&/" $1 -i

