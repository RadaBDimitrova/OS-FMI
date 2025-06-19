#!/bin/bash

#hard to test manually, but logically solved

# if [[ $(whoami) != "oracle" ]] && [[ $(whoami) != "grid" ]]; then
# 	echo "it should be executed by oracle or grid user"
# 	exit 1;
# fi

if [[ $# -ne 1 ]]; then
	echo "exec commands, should be in a whole string,separated by ;"
	exit 2
fi

if ! [[ "${1}" =~ ^[1-9]+[0-9]*$ ]] || [[ "${1}" -lt 2 ]]; then
    echo "should be a number, and should be greater than 1"
    exit 3
fi

if [[ -z "$ORACLE_HOME" ]]; then
	echo "doesn't have an environment variable"
	exit 4
fi

adrcli="${ORACLE_HOME}/bin/adrci"
if [[ ! -f "${adrcli}" ]]; then
    echo "${ORACLE_HOME} doesn't have adrci file"
    exit 5
fi

if [[ ! -x  ]]; then
	echo "${ORACLE_HOME} doesn't have executable adrci file"
	exit 8
fi

diag_dest="/u01/app/$(whoami)/"
minutes=$(( ${1} * 60 ))
setBase="$(echo "${1}" | grep -E "SET BASE ${diag_dest}")"
if [[ -z "${setBase}" ]]; then
	echo "not a valid first set command"
	exit 6
fi

output="$(mktemp)"
${adrcli} exec="SET BASE $diag_dest; SHOW HOMES" > "${output}"
if [[ ${?} -ne 0 ]]; then
    echo "adrcli command failed"
    exit 7
fi

#should be a temp file instead, but I want to show "here string" usage
ofinterest="$(cat "${output}" | tail -n +2 | grep -E "^[^/]+/(crs|tnslsnr|kfod|asm|rdbms)/")"

while read home; do
    ${adrcli} exec="SET BASE ${diag_dest}; SET HOMEPATH ${home}; PURGE -AGE ${minutes}"
done <<< "${ofinterest}"

rm "${output}"