#!/bin/bash
set -o history -o histexpand

# Author: Derrek Woodworth
# This script builds the copy program and tests it

DATA='Here is text that is in a file' 

# Build the source code
gcc -o copy2 copy2.c


# --- Test case 1 ---
# Source file does not exist
# Destination file doesn't exist
# Expected functionality: Program errors and quits
# Supresses error output, if ran on command line it displays error message

# This echo statment is only revalent when the command is run
echo "The following message is the copy command aborting, can't get error redirection working"
./copy2 doesnotexist alsodoesnotexist 


# --- Test case 2 ---
# Source file exists
# Destination file doesn't exist
# Expected functionality: Program copies source to dest

echo $DATA > source.test1

# Running command
./copy2 source.test1 dest.test1


# --- Test case 3 ---
# Source file exists
# Destination file exists
# Expected functionality: Program overwrites source to dest

echo $DATA > source.test2

echo "dummy data" > dest.test2
./copy2 source.test2 dest.test2

# --- Test case 4 ---
# Source directory exists with files
# Destination directory does not exist
# Expected functionality: Program writes source/ to dest/

mkdir source.test3
echo $DATA > source.test3/file1
echo $DATA > source.test3/file2
echo $DATA > source.test3/file3

./copy2 source.test3 dest.test3


# --- Test case 5 ---
# Source dir exists
# Dest file exists
# Expected functionality: Files with the same name are overwritten
cp -r source.test3/ source.test4
cp -r dest.test3/ dest.test4

echo "SHOULD NOT BE TOUCHED" > dest.test4/untouched
echo "SHOULD BE OVERWRITEEN" > dest.test4/file1

./copy2 source.test4/ dest.test4
