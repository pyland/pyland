#!/bin/bash

USAGE="Usage: $0 INPUT_JOB OUTPUT_TILESET OUTPUT_MAP_FML"

if [ -z "$1" ]
then
    echo "You must specifiy an input job file."
    echo "$USAGE"
    exit 1
fi

if [ -z "$2" ]
then
    echo "You must specifiy an output tileset file."
    echo "$USAGE"
    exit 1
fi

if [ -z "$3" ]
then
    echo "You must specifiy an output mapping file."
    echo "$USAGE"
    exit 1
fi

WORK_DIR="/tmp/tile-stitcher-work-$(date +%s)"
MAP_FILE="$WORK_DIR/map.fml"

if mkdir "$WORK_DIR"
then
    ROW_NUMBER=0
    ITEM_NUMBER=0
    ROW_FILES=""
    COLUMN_FILES=""
    while read ROW_LINE
    do
        if [ -n "$ROW_LINE" ]
        then
            ITEM="$ROW_LINE"
            
            ITEM_BASE="$ITEM"
            ITEM_FILE="$ITEM.png"
            
            echo "$ITEM_BASE: $ITEM_NUMBER" >> "$MAP_FILE"
            ITEM_NUMBER=$((ITEM_NUMBER + 1))
            
            COLUMN_FILES="$COLUMN_FILES $ITEM_FILE"
        else
            ROW_FILE="$WORK_DIR/$ROW_NUMBER.png"
            convert $COLUMN_FILES +append $ROW_FILE
            ROW_NUMBER=$((ROW_NUMBER + 1))
            ROW_FILES="$ROW_FILES $ROW_FILE"
            COLUMN_FILES=""
        fi
    done < "$1"

    if [ -n "$COLUMN_FILES" ]
    then
        ROW_FILE="$WORK_DIR/$ROW_NUMBER.png"
        convert $COLUMN_FILES +append $ROW_FILE
        ROW_NUMBER=$((ROW_NUMBER + 1))
        ROW_FILES="$ROW_FILES $ROW_FILE"
    fi

    convert $ROW_FILES -append $2
    mv "$MAP_FILE" "$3"

    rm -r "$WORK_DIR"
else
    echo "Cannot create work directory $WORK_DIR"
    exit 2
fi
