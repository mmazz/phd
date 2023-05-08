#!/bin/bash
mkdir -p data
touch data/time_profile.txt
echo > data/time_profile.txt
value=()
length=10

for i in {2,4,6,8,10,12,14,16,18}
do
    echo "$i"
    my_array=()
    my_array+="$i, "
    for (( c=1; c<=$length; c++ ))
    do
        mapfile -t value < <( ./bin/time "$i" | tail -1 )
        my_array+=$value
    done
    echo $my_array >> data/time_profile.txt
done

touch data/time_profile_degrees.txt
echo > data/time_profile_degrees.txt
for i in {4096,8192,16384,32768}
do
    echo "$i"
    my_array=()
    my_array+="$i, "
    for (( c=1; c<=$length; c++ ))
    do
        mapfile -t value < <( ./bin/time 2 "$i" | tail -1 )
        my_array+=$value
    done
    echo $my_array >> data/time_profile_degrees.txt
done
