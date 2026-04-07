#!/bin/bash

# Disclaimer: logically solved, would be difficult to test locally

# if [[ $(whoami) != "oracle" ]] && [[ $(whoami) != "grid" ]]; then
# 	echo "${0} should be executed by oracle or grid user"
# 	exit 1;
# fi

if [[ $# -ne 1 ]]; then
	echo "exec commands, should be in a whole string,separated by ;"
	exit 2
fi

if ! [[ "${1}" =~ ^[1-9][0-9]*$ ]] || [[ "${1}" -lt 2 ]]; then
    echo "Argument should be a number >= 2"
    exit 3
fi

if [[ -z "$ORACLE_HOME" ]]; then
	echo "doesn't have an environment variable"
	exit 4
fi

adrci="${ORACLE_HOME}/bin/adrci"
if [[ ! -f "${adrci}" ]]; then
    echo "${ORACLE_HOME} doesn't have adrci file"
    exit 5
fi

if [[ ! -x "${adrci}" ]]; then
	echo "${adrci} is not executable"
	exit 6
fi

diag_dest="/u01/app/$(whoami)"
minutes=$(( ${1} * 60 ))

output="$(mktemp)"
${adrci} exec="SET BASE ${diag_dest}; SHOW HOMES" > "${output}"

#should be a temp file instead, but I want to show "here string" usage
ofinterest="$(tail -n +2 "${output}" | grep -E "^[^/]+/(crs|tnslsnr|kfod|asm|rdbms)/")"

if [[ -n "${ofinterest}" ]]; then
    while read home; do
        ${adrci} exec="SET BASE ${diag_dest}; SET HOMEPATH ${home}; PURGE -AGE ${minutes}"
    done <<< "${ofinterest}"
fi

rm "${output}"
