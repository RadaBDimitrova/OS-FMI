# Variables in shell scripts:
let's take $SHELL since we all have it
echo $SHELL{1,2,3} -> nothing, $SHELL is a full string, so brace expansion doesn't work

echo "$SHELL{1,2,3}" -> /bin/bash{1,2,3}

echo ${SHELL}{1,2,3} -> /bin/bash1 /bin/bash2 /bin/bash3
// correct, but what if we had a space? use "" to be safe in these situations:

echo "${SHELL}{1,2,3}" -> /bin/bash1 /bin/bash2 /bin/bash3

# VIM
## Normal mode
gg -> start of buffer
G -> end of buffer
:w -> write to buffer
:wq -> write and quit
:q -> quit
:q! -> quit and don't ask
A -> append at the end
dw -> delete word
<n>w -> move n words
0 -> start of line
dd -> delete line
u -> undo
i -> insert

## Visual mode
In normal mode press 'v':
Select by moving up, down, left and right
y -> (yank) copy
p -> paste
x -> cut

## Insert mode
:)

# ps
-e -> all processes
-u <username> -> processes of username
aux -> in detail (useful for tasks)
-o pid,rss -> custom format (shows pid and rss) and with pid=<newname>,rss=<newname> you can rename the headers + with pid=,rss= you can remove them
--sort <fieldname> -> sort by field

# Redirections

![alt text](https://www.google.com/url?sa=i&url=https%3A%2F%2Feffective-shell.com%2Fpart-2-core-skills%2Fthinking-in-pipelines%2F&psig=AOvVaw1bT5fpjSYEq7Exbfz33RMo&ust=1743273460433000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCMiIve21rYwDFQAAAAAdAAAAABAJ)

> <file> -> redirects output to a file
< <file> -> reads from file
| -> pipe
example:
cat /etc/passwd | head # pipes output of command to the left of the pipe to the input of the command to the right of the pipe

>> <file> -> append (> will clear and rewrite the content, be careful)
1> -> redirect stdout (1>> is possible)
2> -> redirect stdin (2>> is possible)
&> -> redirect stdout and stderr

### NOTE!
2>&1 1 > /dev/null -> redirect 2 to where 1 is currently pointing and 1 to "the black whole" as I call it; 2 goes to the terminal
1 > /dev/null 2>&1 -> redirect 1 to /dev/null and 1 to where 1 is currently pointing so now both point to /dev/null

$() -> command substitution
example:
find "$(pwd)" -> find "<result of pwd>" (in my case pwd's result is "/home/rada" so find "/home/rada" )

# tar
man tar has all flags, but this is what I suggest for exams:
tar -caf <name>.tar.gz <name of directory> -> create archive
tar -xaf <name>.tar.gz -> de-archive it

# head
-n N -> first N rows  -N and +N possible

# tail
-n N -> last N rows  
-N and +N possible

# wc
-l -> count lines  
-w -> count words  
-c -> count bytes  
example:  
wc -l <file> -> count lines in a file  

# cut
-d <delimiter> -> specify delimiter (default is tab)  
-f <fields> -> select fields  
example:  
echo "a:b:c" | cut -d ':' -f 2 -> b  
echo "a:b:c" | cut -d ':' -f 2- -> b:c 
echo "a:b:c" | cut -d ':' -f -2 -> a:b 

# tr
-d <chars> -> delete characters  
-s <chars> -> squeeze repeated characters  
example:  
echo "hello   world" | tr -s ' ' -> hello world  
echo "abc" | tr 'a-z' 'A-Z' -> ABC  

# sort
-n -> numeric sort  
-r -> reverse sort  
-k <key> -> sort by specific key 
-t <separator> -> field separator 
example:  
sort -n -k 2 <file> -> sort numerically by the second column  

# uniq
-c -> count occurrences   
example:  
uniq -c <file> -> count occurrences of each line  

# grep
-i -> case insensitive  
-v -> invert match  
-c -> count matches  
example:  
grep -i "pattern" <file> -> search for "pattern" ignoring case  
grep -v "pattern" <file> -> show lines that do not match "pattern"  


