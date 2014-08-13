#!/bin/bash

USAGE="Usage: $0 SCAN_DIRECTORY OUTPUT_JOB"

if [ -z "$1" ]
then
    echo "You must specifiy a directory."
    echo "$USAGE"
    exit 1
fi

if [ -z "$2" ]
then
    echo "You must specifiy an output file."
    echo "$USAGE"
    exit 1
fi

WORK_DIR="/tmp/tile-gen-job-work-$(date +%s)"
LIST_FILE="$WORK_DIR/tiles.lst"
JOB_FILE="$WORK_DIR/tiles.job"

if mkdir "$WORK_DIR"
then
    find "$1" -iname '*.png' > "$LIST_FILE"

    FIRST_DIR=${2%%/}
    if [ -n "$FIRST_DIR" ]
    then
        FIRST_DIR="."
    fi

    while read ITEM
    do
        if [ -z "$ITEM" ]
        then
            echo "" >> "$JOB_FILE"
        else
            if [ "$FIRST_DIR" == '.' ]
            then
                NO_PRE=${ITEM#*/}
                BASE=${NO_PRE%.*}
                echo "$BASE" >> "$JOB_FILE"
            else
                BASE=${ITEM%.*}
                echo "$BASE" >> "$JOB_FILE"
            fi
        fi
    done < "$LIST_FILE"

    sort -V "$JOB_FILE" > "$2"

    if (grep ' ' "$2" > "$WORK_DIR/spaces.lst")
    then
        echo "Warning: Tile names are unsanitised."
        cat "$WORK_DIR/spaces.lst"
    fi
    
    rm -r "$WORK_DIR"
else
    echo "Cannot create work directory $WORK_DIR"
    exit 2
fi
