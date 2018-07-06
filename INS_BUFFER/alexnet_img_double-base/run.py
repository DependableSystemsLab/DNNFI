#!/usr/bin/python

import os, sys

inputImgPath = sys.argv[1]
eachFiRun = int(sys.argv[2])

os.system("python profile_wrapper.py ./caffe_converter/caffe_converter.cpp \"../common/bvlc_alexnet/deploy.prototxt ../common/bvlc_alexnet/bvlc_alexnet.caffemodel ../common/ilsvrc12/imagenet_mean.binaryproto ../common/ilsvrc12/synset_words.txt " + inputImgPath + " \"")

os.system("python inject_wrapper.py ./caffe_converter/caffe_converter.cpp \"../common/bvlc_alexnet/deploy.prototxt ../common/bvlc_alexnet/bvlc_alexnet.caffemodel ../common/ilsvrc12/imagenet_mean.binaryproto ../common/ilsvrc12/synset_words.txt " + inputImgPath + " " + `eachFiRun` + " \"")
