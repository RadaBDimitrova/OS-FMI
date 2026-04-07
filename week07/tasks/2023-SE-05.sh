#!/bin/bash

ps_res=$(mktemp)
cmds=$(mktemp)
count=0


while true; do
        ps -e -o comm=,rss= > "${ps_res}"
        count=$(( count + 1 )) # (( count++ ))
        BREAK=false
        while read comm; do
            sum="$( grep -E "^${comm}\s" "${ps_res}" | awk '{print $2}' | xargs | tr ' ' '+' | bc)" # xargs way
            # alternative: sum=$(awk -v cmd="${comm}" '$1 == cmd {s+=$2} END {print s+0}' "${ps_res}")
                if [[ "${sum}" -gt 65536 ]]; then
                        echo "${comm}" >> "${cmds}"
                        BREAK=true
                fi
        done < <( awk '{print $1}' "${ps_res}" | sort | uniq)

        if [[ "${BREAK}" == false ]]; then
            break
        fi
        sleep 1
done

sort "${cmds}" | uniq -c | awk -v count="${count}" '$1 > count/2 {print $2}' # conditional print in awk without if
rm "${ps_res}" "${cmds}"
