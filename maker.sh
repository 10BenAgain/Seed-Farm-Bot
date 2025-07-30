#!/usr/bin/bash

SAV=$1
SAVS=/savs
NAMER=/name.py
PULLER=/pull.exe
RESULTS=/Results
PY=python

# Grabs latest start point and button setting from Makefile
START=$(awk -F':=' '/^START/ { gsub(/[ \t]/, "", $2); print $2 }' Makefile)
BUTTON=$(awk -F':=' '/^BUTTON/ { gsub(/[ \t]/, "", $2); print $2 }' Makefile)

exec=$(PWD)$SAVS$NAMER
output=$(PWD)$SAVS$RESULTS

$PY $exec $SAV $output $START $BUTTON

# Get most recent file name
latest=$(cd $output; ls -t | head -1)

$(PWD)$PULLER $output/$latest
    echo Done.


