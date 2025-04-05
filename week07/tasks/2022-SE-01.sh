#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "config file needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "arg isn;t a file"
    exit 2
fi

procfile="wakeup" # analogical to /proc/wakeup in my case
workfile=$(mktemp)

while read line; do
    # checks if all devices are with max len 4 and if values are enabled or disabled
    line="$(echo "${line}" | sed -E 's/(^[^#]*)#.*$/\1/g' | tr -s ' ')"
    if [[ -n "${line}" ]]; then
        f1="$(echo "${line}" | awk '{print $1}')"
        len=${#f1}
        if [[ $len -gt 4 ]]; then
            echo "${f1} - device name shoulld be 4 characters max"
            rm "${workfile}"
            exit 2
        fi

        if ! grep -qE "^${f1}" "${procfile}"; then
            echo "${f1} doesn't exist in proc file"
            rm "${workfile}"
            exit 3
        fi

        f2="$(echo "${line}" | awk '{print $2}' | grep -E "^(disabled|enabled)$")"
        if [[ -n "${f2}" ]]; then
            echo "${line}" >> "${workfile}"
        else
            echo "${f2} - values can only be 'enabled' or 'disabled'"
            rm "${workfile}"
            exit 4
        fi
    fi
done < "${1}" # <( cat "${1}" | sed -E 's/(^[^#]*)#.*$/\1/g') is possible instead of cleaning up the line on L18

while read line; do
    f1="$(echo "${line}" | awk '{print $1}')"
    f2="$(echo "${line}" | awk '{print $2}')"
    current="$( grep -E "^${f1}\b" "${procfile}" | awk '{print $3}' | tr -d '*')"
    echo "$f1 $f2 $current"
    if [[ "${current}" != "${f2}" ]]; then
        echo "replacing:"
        # echo "${f1}" > "${procfile}"
        sed -i -E "s/(^${f1}.*)${current}/\1${f2}/" "${procfile}"
    fi
done < "${workfile}"


rm "${workfile}"