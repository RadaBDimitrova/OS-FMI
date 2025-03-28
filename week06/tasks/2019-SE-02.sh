#!/bin/bash


START_TIME=1551168000
END_TIME=1551176100

# I will use command substitution "$(date)" to get the current time so as to show something
changed_dirs=$(find /home/students -maxdepth 1 -type d -newerct "@${START_TIME}" ! -newerct "$(date)")


while read line; do
    username="$(echo "${line}" | cut -d ':' -f 1)"
    name="$(echo "${line}" | cut -d ':' -f 5 | cut -d ',' -f 1)"
    home="$(echo "${line}" | cut -d ':' -f 6)"
    if echo "${changed_dirs}" | grep -q "${home}"; then
        fn=$(echo "${username}" | grep -oE '[0-9]+')
        echo -e "${fn}\t${name}"
    fi
done < /etc/passwd