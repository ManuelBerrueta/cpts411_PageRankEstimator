#!/bin/sh
#To run this script, please use "sbatch -N<numberofnodes> <path>/sub.sh"

# setting the max wallclock time after which the job will be killed; 
# note: the format is hr:min:sec (default set here to 10 mins)
#SBATCH --time=00:05:00

#"np" stands for number of processes.
#this command will run the job on n processes. 
# $1=k (# of Walks), $2=D (is damping Ratio), $3=<inpuit fileName>
# $4=t (# of Threads) and $5=nSize (Highest node value)
# sample usage: ./jobscript.sh <p> <n>
#   sbatch -N1 jobscript.sh 8 1024
mpirun -np 1 ~/cpts411_PageRankEstimator/pagerankestimator $1 $2 $3 $4 $5 > output.txt

# you can add more lines of commands here, if you wish.