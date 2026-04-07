#!/bin/bash

variables=""
args=""
filename=""
sawJar=0
foundFilename=0

for arg in "${@}"; do
	if [[ "${arg}" == "-jar" ]]; then
		sawJar=1
		continue
	fi

	if [[ ${foundFilename} -eq 1 ]]; then
		args="${args} ${arg}"
		continue
	fi

	if [[ "${arg}" =~ ^-D.+= ]]; then
		if [[ ${sawJar} -eq 1 ]]; then
			variables="${variables} ${arg}"
		fi
		continue
	fi

	# first non-option argument after -jar is filename
	if [[ ${sawJar} -eq 1 ]] && [[ "${arg}" != -* ]]; then
		filename="${arg}"
		foundFilename=1
		continue
	fi
done

if [[ ${sawJar} -eq 0 ]]; then
    echo "No -jar option provided"
    exit 1
fi

if [[ ${foundFilename} -eq 0 ]]; then
    echo "No JAR filename provided after -jar"
    exit 2
fi

if [[ ! -f "${filename}" ]]; then
    echo "File ${filename} does not exist"
    exit 3
fi

echo "java ${variables} -jar ${filename} ${args}"
# eval "java ${variables} -jar ${filename} ${args}"
