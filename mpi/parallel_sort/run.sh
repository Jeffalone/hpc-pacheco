#!/usr/bin/env bash

#SBATCH --job-name=helloMPI
#SBATCH --error=job.%J.err
#SBATCH --output=job.%J.out.128
#SBATCH --ntasks=64
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=100

module load gcc
module load openmpi

cd ~/mpi/parallel_sort/
mpiexec -n 64 ./ms <<< '128000000'
