#!/bin/bash

cmds=$(mktemp)
count=0

# could store ps result in a temp file or a variable
ps_res=$(mktemp)
ps -e -o comm=,rss= > "${ps_res}"

while true; do
        count=$((count + 1))
        BREAK=false
        while read comm; do
            sum="$(cat "${ps_res}" | grep -E "^${comm}" | awk '{print $2}' | xargs | tr ' ' '+' | bc)" # xargs
                if [[ "${sum}" -gt 65530 ]]; then
                        echo "${comm}" >> "${cmds}"
                        BREAK=true
                fi

        done < <(cat "${ps_res}" | awk '{print $1}' | sort | uniq)

        if [[ "${BREAK}" == false ]]; then
            break
        fi
        sleep 1
done

cat "${cmds}" | sort | uniq -c | awk -v count="${count}" '$1 > count/2' #conditional print in awk without if
rm "${ps_res}"
rm "${cmds}"