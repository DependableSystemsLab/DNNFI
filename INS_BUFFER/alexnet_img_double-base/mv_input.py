#! /usr/bin/python

import os, sys, random

fiNo = sys.argv[1]
inputDir = sys.argv[2]

# Moving image randomly chosen
imgList = os.listdir(inputDir)
imgName = random.choice(imgList)
os.system( "cp " + inputDir + imgName + " ./input.jpeg")

# Record the image name
os.system(" echo \"" + imgName + "\" > input_name.txt-" + fiNo)

