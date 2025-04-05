#!/bin/bash

if [[ ${#} -ne 1 ]]; then
        echo "Argument count should be 1";
        exit 1;
fi

if ! echo "${1}" | grep -q -E  "^([1-9]*[0-9]+)|0$" ; then
        echo "Argument should be a number";
        exit 2;
fi

#if [[ "$(whoami)" != "root" ]]; then
#       echo "not root"
#       exit 3;
#fi

# could use a variable or a temp file to store the output of ps
table="$(ps -eo uid=,pid=,rss=)"
users="$(echo "${table}" | awk '{print $1}' | sort -k 1 | uniq)"

for user in ${users}; do
    total_rss="$(ps -u "${user}" -o rss= | awk '{sum+= $1} END {print sum} ')"
echo "${user} has total rss: ${total_rss}"
if [[ "${total_rss}" -gt "${1}" ]] then
        highest_pid="$(ps -u "${user}" -o pid=, --sort rss | tail -n 1)"
    #   kill "$(highest_pid)"
fi
done