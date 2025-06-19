#!/bin/bash

variables=$(mktemp)
filename=""
args=$(mktemp)
jar="-jar"
lastArgWasJar=0
lastArgWasName=0
valid=0
for arg in "${@}"; do
	#-jar check
	if [[ "${arg}" == "java" ]]; then
		valid=1
		continue
	fi
	if [[ "${arg}" == "${jar}" ]]; then
		lastArgWasJar=1
		continue
	fi
	if echo "${arg}" | grep -Eq "-D(.*)=" ; then
		#variables
		if [[ ${lastArgWasJar} -eq 1 ]]; then
			echo "${arg}" >> "${variables}"
		else
			#invalid placement to valid one with default
			def="$( echo "${arg}" | cut -d '=' -f 1)"
			echo "${def}=default" >> "${variables}"
		fi
		continue
	fi
	#filename after -jar check
	if [[ ${lastArgWasJar} -eq 1 ]]; then
		filename="${arg}"
		lastArgWasName=1
		lastArgWasJar=0
		continue
	elif [[ ${lastArgWasName} -eq 1 ]]; then
		#args check
		lastArgWasJar=0
		echo "${arg}" >> "${args}"
		continue
	fi
done

vars="$(cat "${variables}" | tr '\n' ' ')"
arguments="$(cat "${args}" | tr '\n' ' ')"

if [[ ${valid} -eq 1 ]]; then
	echo "java ${vars} ${jar} ${filename} ${arguments}"
fi

rm "${args}"
rm "${variables}"
