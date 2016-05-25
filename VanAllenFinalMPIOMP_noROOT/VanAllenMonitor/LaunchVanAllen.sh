
#!/bin/bash

#  LaunchVanAllen.sh
#  VanAllenMonitor3
#
#  Created by Gabriele Gaetano Fronzé on 21/04/15.
#  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.

set -m

MPIoptions=$(cat VanAllenConfig.config | grep -i '^MPIoptions' | cut -f2 -d$'\t')
NelLow=$(cat VanAllenConfig.config | grep -i '^NelLow' | grep -Eo '[0-9]{1,}')
NelHi=$(cat VanAllenConfig.config | grep -i '^NelHi' | grep -Eo '[0-9]{1,}')
Multi=$(cat VanAllenConfig.config | grep -i '^Multi' | grep -Eo '[0-9]{1,}')
Monitor=$(cat VanAllenConfig.config | grep -i '^Monitor' | grep -ci 'true')
Seed=$(cat VanAllenConfig.config | grep -i '^Seed' | grep -Eo '[0-9]{1,}')
Master=$(cat VanAllenConfig.config | grep -i '^Master' | grep -Eo '[0-9]{1,}')
Finalizer=$(cat VanAllenConfig.config | grep -i '^Finalizer' | grep -Eo '[0-9]{1,}')

if [ $Monitor -eq 1 ]
then
echo "starting monitor"
./Monitor &
jobnumber=$(jobs | grep './Monitor &' | grep -o '\[[0-9]*\]' | cut -d "[" -f2 | cut -d "]" -f1)
fi

mpiexec $MPIoptions VanAllen $NelLow $NelHi $Multi $Monitor $Seed $Master $Finalizer &

if [ $Monitor -eq 1 ]
then
fg $jobnumber
fi
