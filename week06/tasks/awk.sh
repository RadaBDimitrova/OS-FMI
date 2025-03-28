#!/bin/bash

#-v
name="OS"
echo "" | awk -v course="$name" '{print "This is the course name:", course }'

#FS and OFS
## FS
cat a | awk 'BEGIN {FS=","} {print "ID:", $1, "Second field:", $2}'

#incorrect but why? ($0 is the whole line)
cat a | awk 'BEGIN {FS=","; OFS=";" } {print "NEW: ", $0}'

#correct
cat a | awk 'BEGIN {FS=","; OFS=";" } {print "NEW: ", $1, $2, $4}'

#additional (from 2nd field onwards without 4th field - for cycles and checks in awk)
cat a | awk 'BEGIN {FS=","; OFS=" | "} {for (i=2; i<=NF; i++) if ($i != "4") printf "%s%s", $i, (i<NF ? OFS : "\n")}'