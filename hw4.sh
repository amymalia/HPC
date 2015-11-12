#!/bin/sh

#  hw3.sh
#
#
#  Created by Amy Takayesu on 10/4/15.
#

module load tools/simgrid/3.13_20151014

echo "------------------------PART 1------------------------"
smpicc -o bcast_default bcast_skeleton.c -DDEFAULT
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform ring_100.xml -hostfile hostfile_100 ./bcast_default -c 100000 -s DEFAULT

smpicc -o bcast_naive bcast_skeleton.c -DNAIVE
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform ring_100.xml -hostfile hostfile_100 ./bcast_naive -c 100000 -s NAIVE

smpicc -o bcast_ring bcast_skeleton.c -DRING
smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform ring_100.xml -hostfile hostfile_100 ./bcast_ring -c 100000 -s RING

echo "------------------------PART 2------------------------"

smpicc -o bcast_ring_pipelined bcast_skeleton.c -DRING_PIPELINE

chunk="100000 200000 500000 1000000 2000000 10000000 100000000"
processor="25 50 100"
for i in $chunk
do
    for j in $processor
    do
    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np $j -platform ring_$j.xml -hostfile hostfile_$j ./bcast_ring_pipelined -c $i -s CHUNK:$i___SIZE:$j
    done
done

echo "------------------------PART 3------------------------"

smpicc -o bcast_ring_pipelined_isend bcast_skeleton.c -DRING_PIPELINE_ISEND

chunk="100000 200000 500000 1000000 2000000 10000000 100000000"
processor="25 50 100"
for i in $chunk
do
    for j in $processor
    do
    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np $j -platform ring_$j.xml -hostfile hostfile_$j ./bcast_ring_pipelined_isend -c $i -s CHUNK:$i___SIZE:$j
    done
done

echo "------------------------PART 4------------------------"

smpicc -o bcast_bintree_pipelined_isend bcast_skeleton.c -DBIN_TREE

chunk="100000 200000 500000 1000000 2000000 10000000 100000000"
for i in $chunk
do
    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform bintree_100.xml -hostfile hostfile_100 ./bcast_bintree_pipelined_isend -c $i -s CHUNK:$i___BINTREE

    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform bintree_100.xml -hostfile hostfile_100 ./bcast_ring_pipelined_isend -c $i -s CHUNK:$i___RING
done

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 100 -platform bintree_100.xml -hostfile hostfile_100 ./bcast_default -c 100000 -s DEFAULT

echo "------------------------PART 5------------------------"

chunk="100000 200000 500000 1000000 2000000 10000000 100000000"

for i in $chunk
do
    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform cluster_crossbar_64.xml -hostfile hostfile_64 ./bcast_bintree_pipelined_isend -c $i -s CHUNK:$i___CROSSBAR____BINTREE

    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform fattree_64.xml -hostfile hostfile_64 ./bcast_bintree_pipelined_isend -c $i -s CHUNK:$i___FATTREE____BINTREE

    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform cluster_crossbar_64.xml -hostfile hostfile_64 ./bcast_ring_pipelined_isend -c $i -s CHUNK:$i___CROSSBAR____RING

    smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform fattree_64.xml -hostfile hostfile_64 ./bcast_ring_pipelined_isend -c $i -s CHUNK:$i___FATTREE____RING
done

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform cluster_crossbar_64.xml -hostfile hostfile_64 ./bcast_default -s DEFAULT___CROSSBAR

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np 64 -platform fattree_64.xml -hostfile hostfile_64 ./bcast_default -s DEFAULT____FATTREE
