#!/bin/bash

# showcase of using awk to sum digits and using mktemp

nums=$(mktemp)
while read line; do
        (echo "${line}" | grep -E "^-?([1-9]+[0-9]*|0)$") >> $nums
done

maxNum=$(cat "${nums}" | tr -d '-' | sort -n | tail -1)

#a)
cat $nums | grep -E "$maxNum" | sort | uniq

#b)

digitNums=$(mktemp)
while read num; do
    digitSum="$(echo $num | grep -E -o "[0-9]" | awk '{sum+=$1} END {print sum}')"
    (echo "$num,$digitSum") >> $digitNums
done < $nums


maxSum=$(cat "${digitNums}" | sort -t ',' -k 2 -n | cut -d ',' -f 1 | tail -1)
cat "${digitNums}" | grep -E ",${maxSum}" | sort -t ',' -k 1 -n | cut -d ',' -f 1 | head -1

# important to remove temp files
rm "${nums}"
rm "${digitNums}"