#Assignment 1 directory

This directory contains source code and other files for Assignment 1.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.

program needs to accept 1 input
either set, get, or invalid
file names also need to be valid, less then PATH_MAX and no \0
set:
user types set and the name of the file and then enters a new line
rest of what they write will be written in the file
ends when they do ctrl-d
program writes OK to stdout
return code 0
get:
user types get and name of file
for get to open a file, it already needs to exist
contents of file written to stdout
return code 0
invalid command:
user types anything else, or if they dont write get or set correctly
invalid command written to stderr
return code 1