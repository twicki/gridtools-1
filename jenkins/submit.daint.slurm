#!/bin/bash
#SBATCH --job-name=gridtools_test
#SBATCH --nodes=<MPI_NODES>
#SBATCH --ntasks=<MPI_TASKS>
#SBATCH --ntasks-per-node=<MPI_PPN>
#SBATCH --output=<OUTPUTFILE>
#SBATCH --time=00:25:00
#SBATCH -C gpu
#SBATCH --cpus-per-task=<CPUSPERTASK>
#SBATCH --account=c14
#SBATCH --partition=<QUEUE>

echo $LD_LIBRARY_PATH

# set environmental parameters

<JOB_ENV>

<CMD>
