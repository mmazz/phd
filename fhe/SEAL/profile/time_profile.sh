#!/bin/bash
touch time_profile.txt
declare -a values=(2,4,6,8,10,12,14,16,18)
echo "pow_values = [2, 4, 6, 8, 10, 12, 14, 16, 18]" > time_profile.txt

echo "time_values [" >> time_profile.txt
for i in {2,4,6,8,10,12,14,16,18}
do
    echo "$i"
    ./bin/time "$i" | tail -1 >> time_profile.txt
done
echo "]" >> time_profile.txt

touch time_profile_degree.txt

echo "time_values [" > time_profile_degree.txt
for i in {4096,8192,16384,32768}
do
    echo "$i"
    ./bin/time 2 "$i" | tail -1 >> time_profile_degree.txt
done
echo "]" >> time_profile_degree.txt
