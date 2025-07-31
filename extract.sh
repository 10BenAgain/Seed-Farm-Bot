#!/usr/bin/bash

# This repos file sctructure 
ROOT=$(pwd)
SAVS="savs"
BIN="bin"

# Script argument takes in path to .sav file
FILE_PATH=$1

if [ -z "$FILE_PATH" ]; then
    echo "Please specify a target file!"
    exit
fi

FILE_PATH=$(realpath $1)

# Local path for pull binary
GOP="pull"
PULL="$BIN/$GO"

# Local path for naming script
SCRIPT="name.py"
NAMER=$SAVS/$SCRIPT

# Where to store the renamed file
RESULTS="$SAVS/results"

# SSH client details; Assumes that ssh token is already established on both devices
USER_NAME="ben"
CLIENT="rpi5"
REPO_DIR="/home/ben/seedfarm/fire_red_seeds/carduino"

# Holy escape sequences... 
echo "Fetching values from remote client's Makefile..."
START=$(ssh "$USER_NAME@$CLIENT" "cd $REPO_DIR && awk -F':=' '/^START/ { gsub(/[[:space:]]/, \"\", \$2); print \$2 }' Makefile")
    echo "  -> Found start value: $START"

BUTTON=$(ssh "$USER_NAME@$CLIENT" "cd $REPO_DIR && awk -F':=' '/^BUTTON/ { gsub(/[[:space:]]/, \"\", \$2); print \$2 }' Makefile")
    echo "  -> Found button: $BUTTON"

# Run python script to generate properly named .sav file
python $ROOT/$NAMER $FILE_PATH $ROOT/$RESULTS $START $BUTTON

latest=$(cd $ROOT/$RESULTS; ls -t | head -1)

$ROOT/$PULL/$GOP $ROOT/$RESULTS/$latest

echo "Done."