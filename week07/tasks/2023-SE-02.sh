#!/bin/bash

if [[ ${#} -lt 2 ]]; then
    echo "at least 2 args needed"
    exit 1
fi

if ! echo "${1}" | grep -qE "([1-9]+[0-9]*)|0"; then
        echo "first arg should be a number"
        exit 2;
fi

sec="${1}"
com="$(echo "${@}" | cut -d ' ' -f 1 --complement)" # could use shift

allSec=0
counter=0
while [[ "$(echo "${allSec}" | cut -d '.' -f 1)" -lt "${sec}" ]]; do
        startTime="$(date +%s.%N | head -c 13)"
        # eval "${com}" &>/dev/null
        $(${com}) &>/dev/null
        endTime="$(date +%s.%N | head -c 13)"
        execSec="$(echo "scale=2; $endTime - $startTime" | bc)"
        counter=$((counter+1))
        allSec=$(echo "$execSec + $allSec" | bc)
done

echo "Ran the command ${com} ${counter} times for ${allSec} seconds."
echo "Average runtime: $(echo "scale=2; ${allSec}/${counter}" | bc) seconds."