#!/bin/bash

dict="$(mktemp)"
files="$(mktemp)"

for arg in "${@}"; do
    if echo "${arg}" | grep -Eq "^-R[[:alnum:]]+=[[:alnum:]]+"; then
        echo "${arg}" | sed -E 's/-R([[:alnum:]]+=[[:alnum:]]+)/\1/' >> "${dict}"
    elif echo "${arg}" | grep -Eqv "-" && [[ -f "${arg}" ]]; then
        echo "${arg}" >> "${files}"
    else
        echo "invalid arg: ${arg}"
        exit 1
    fi
done

separator="$(pwgen | cut -f 1)"
while read filename; do
    while read entry; do
        replace="$(echo "${entry}" | cut -d '=' -f 1)"
        replacement="$(echo "${entry}" | cut -d '=' -f 2)"
        sed -iE "/^#/! s/\b${replace}\b/${replacement}${separator}/g" "${filename}"
       # note: this ^ is an address pattern, otherwise pseudocode: if contains #; then echo >> tmp (a mktemp file); else echo | sed >> tmp ; && mv tmp filename
    done < "${dict}"
    sed -iE "s/${separator}//g" "${filename}"
done < "${files}"


rm "${dict}"
rm "${files}"