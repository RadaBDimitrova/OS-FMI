#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "2 arguments needed"
    exit 3;
fi

if [[ ! -f "${1}" ]]; then
    echo "Source file doesn't exist"
    exit 1;
fi

if [[ -f "${2}" ]]; then
    echo "Dest file already exists, chose another name"
    exit 2;
fi

while read line; do
    copies="$(cat "${1}" | grep -E "${line}")"
    if [[ $(echo "${copies}" | wc -l ) -gt 1 ]]; then # show different way to use grep in a test [[]]
        smallestId="$(echo "${copies}" | cut -d ',' -f 1 | sort -n | head -n 1)"
        (echo "$smallestId,$line") >> "${2}"
    else
        (echo "${copies}") >> "${2}"
    fi
done < <(cat "${1}" | cut -d ',' -f 1 --complement | awk '{print ","$0}' | sort | uniq ) # to show --complement