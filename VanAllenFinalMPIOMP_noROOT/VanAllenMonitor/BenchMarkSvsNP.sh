#!/bin/sh

#  BenchMark2.sh
#  VanAllenFinalMPIOMP
#
#  Created by Gabriele Gaetano Fronzé on 27/04/15.
#  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.

NRUNS=5
NPMAX=10
declare -a NEL=("100" "400" "800" "2000" "4000")

file=dummy.csv
results=res2.csv
measuremins=0
measuresecs=0
dummy=0;

if [[ -n $(find $results) ]]
then
rm $results
fi

if [[ -n $(find $file) ]]
then
rm $file
fi

echo -ne "SEQUENTIAL\nElectron number;;" >> $results
for i in "${NEL[@]}"
do
    echo -ne "\n$i;;" >> $results
    for (( c=1; c<=$NRUNS; c++ ))
    do
#{ time echo "$i" "$j" ; } 2> $file
{ time ./VanAllenSeq "$i" ; } 2> $file
        if [[ -n $(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'}) ]]
        then
            measuremins=$(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'})
        else
            measuremins=0
        fi
        measuresecs=$(cat $file | grep -i "^real" | grep -Eo '[0-9.]*s' | grep -Eo '[0-9.]{1,}')
        dummy=$( echo "$measuremins*60 + $measuresecs" | bc )
        echo "$dummy"
        echo -n "$dummy;" >> $results
    done
done

echo -ne "\n\nPARALLEL M Only\nElectron number;MPI Process Number;Multiplier=10;" >> $results

for i in "${NEL[@]}"
do
    for (( j=3; j<=$(($NPMAX+1)); j++ ))
    do
        echo -e "\n$i;$j;"
        echo -ne "\n$i;$j;" >> $results
        for (( c=1; c<=$NRUNS; c++ ))
        do
#{ time echo "$i" "$j" ; } 2> $file
{ time time mpiexec -np "$j" VanAllen 0 "$i" 10 0 1234 0 0 ; } 2> $file
            if [[ -n $(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'}) ]]
            then
                measuremins=$(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'})
            else
                measuremins=0
            fi
            measuresecs=$(cat $file | grep -i "^real" | grep -Eo '[0-9.]*s' | grep -Eo '[0-9.]{1,}')
            dummy=$( echo "$measuremins*60 + $measuresecs" | bc )
            echo "$dummy"
            echo -n "$dummy;" >> $results
        done
    done
done

echo -e "\n" >> $results



echo -ne "PARALLEL M+F\nElectron number;MPI Process Number;Multiplier=10;" >> $results

for i in "${NEL[@]}"
do
    for (( j=2; j<=$NPMAX; j++ ))
    do
        echo -e "\n$i;$j;"
        echo -ne "\n$i;$j;" >> $results
        for (( c=1; c<=$NRUNS; c++ ))
        do
#{ time echo "$i" "$j" ; } 2> $file
{ time time mpiexec -np "$j" VanAllen 0 "$i" 10 0 1234 0 ; } 2> $file
            if [[ -n $(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'}) ]]
            then
                measuremins=$(cat $file | grep -i "^real" | grep -Eo '[0-9]*m' | grep -Eo '[0-9]{1,}'})
            else
                measuremins=0
            fi
            measuresecs=$(cat $file | grep -i "^real" | grep -Eo '[0-9.]*s' | grep -Eo '[0-9.]{1,}')
            dummy=$( echo "$measuremins*60 + $measuresecs" | bc )
            echo "$dummy"
            echo -n "$dummy;" >> $results
        done
    done
done

echo -e "\n" >> $results

rm $file