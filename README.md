# DNNFI
Fault Injector for Deep Neural Network (DNN) Accelerators

Setup:
1. Download model file from BVLC. For example, download bvlc_alexnet.caffemodel and place it in PE/common/bvlc_alexnet folder.
2. Download image set for training and testing. For example, ImageNet dataset.
3. Configure the driver files with the proper paths of the dataset and fault injection configurations etc.
4. Run the driver file to do fault injections.

Paper:

http://blogs.ubc.ca/karthik/files/2017/12/DNN-SC17.pdf


Citation:

@inproceedings{li2017understanding,
  title={Understanding error propagation in deep learning neural network (DNN) accelerators and applications},
  author={Li, Guanpeng and Hari, Siva Kumar Sastry and Sullivan, Michael and Tsai, Timothy and Pattabiraman, Karthik and Emer, Joel and Keckler, Stephen W},
  booktitle={Proceedings of the International Conference for High Performance Computing, Networking, Storage and Analysis},
  pages={8},
  year={2017},
  organization={ACM}
}
