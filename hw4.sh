#!/bin/sh

#  hw3.sh
#
#
#  Created by Amy Takayesu on 10/4/15.
#

smpicc matmul_outerproduct_v1.c

echo "------------------------P1------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 1 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out

echo "------------------------P4------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 4 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out

echo "------------------------P16------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 16 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out

echo "------------------------P64------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 64 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out

echo "------------------------P100------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 100 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out

echo "------------------------P400------------------------"
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.054Gf -np 400 -platform cluster_1600.xml -hostfile hostfile_1600 ./a.out
