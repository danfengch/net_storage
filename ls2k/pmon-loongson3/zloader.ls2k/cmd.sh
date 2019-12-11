#!/bin/bash
export PATH=/home/chenxu/LOONGSON-2k1000/toolchain/gcc-4.4-gnu/bin/:$PATH
make cfg all tgt=rom CROSS_COMPILE=mipsel-linux- DEBUG=-g 
#make tgt=rom CROSS_COMPILE=mipsel-linux- DEBUG=-g 
make dtb

