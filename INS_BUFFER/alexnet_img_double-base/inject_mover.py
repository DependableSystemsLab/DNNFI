#! /usr/bin/python

import os, sys, random

fiNo = sys.argv[1]
inputDir = sys.argv[2]

print "\n**** Moving FI logs ... **** \n"

# For standard FI info log.
os.system("mv golden_result.txt cfi/baseline/golden_result.txt-" + fiNo)
os.system("mv fi_log cfi/fi_log/fi_log-" + fiNo)

# For calculated data.
os.system("mv last_layer_a.txt cfi/prog_output/last_layer_a.txt-" + fiNo)
os.system("mv last_layer_out.txt cfi/prog_output/last_layer_out.txt-" + fiNo)
os.system("mv each_layer_euclidean.txt cfi/prog_output/each_layer_euclidean.txt-" + fiNo)
os.system("mv each_layer_diff_time.txt cfi/prog_output/each_layer_diff_time.txt-" + fiNo)
os.system("mv each_layer_spread.txt cfi/prog_output/each_layer_spread.txt-" + fiNo)
os.system("mv result.txt cfi/prog_output/result.txt-" + fiNo)
os.system("mv input_name.txt-" + fiNo + " cfi/baseline/")
