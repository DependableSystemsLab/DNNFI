#! /usr/bin/python

import os, sys, time

##############################################
fiLayerNo = 1 # 1, this is conv layer 1 for alexnet
minInst = 0
maxInst = 5
eachFiNo = 1000 # fi number for each image input
projectRoot = os.path.dirname(os.path.realpath(__file__))
##############################################


for y in range(minInst, maxInst+1): 
	os.system("cp -r alexnet_img_double-base alexnet_img_double_" + `fiLayerNo` + "_" + `y`)
	os.chdir("alexnet_img_double_" + `fiLayerNo` + "_" + `y`)
	os.system("./caffe_converter.exe " + projectRoot + "/common/bvlc_alexnet/deploy.prototxt " + projectRoot+ "/common/bvlc_alexnet/bvlc_alexnet.caffemodel " + projectRoot + "/common/ilsvrc12/imagenet_mean.binaryproto " + projectRoot + "/common/ilsvrc12/synset_words.txt input.jpeg " + `eachFiNo` + " " + projectRoot + "/../../imagenet/ " + `fiLayerNo` + " " + `y`)
	os.chdir("../")	
