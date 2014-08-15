#!/bin/bash

USAGE="Usage: $0 NUMERIFY_DIR"

if [ -z "$1" ]
then
    echo "You must specifiy a directory."
    echo "$USAGE"
    exit 1
fi

WORK_DIR="/tmp/tile-numerify-work-$(date +%s)"
LIST_FILE="$WORK_DIR/tiles.lst"

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
        # Directory
        ITEM_BASE=$(expr match "$ITEM" '\(.*/\).*')
        if [ -z "$ITEM_BASE" ]
        then
            ITEM_NEW=$(expr match "$ITEM" '[^0-9]*\([0-9]*\.png\)')
        else
            # Get rid of directory for regexing
            ITEM_NAME=$(expr match "$ITEM" '.*/\(.*\)')
            ITEM_NEW="$ITEM_BASE$(expr match "$ITEM_NAME" '[^0-9]*\([0-9]*\.png\)')"
        fi
        if [ "$ITEM" != "$ITEM_NEW" ]
        then
            read -p "Confirm: $ITEM    ->    $ITEM_NEW: " -n 1 -r <&1
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]
            then
                mv -i "$ITEM" "$ITEM_NEW"
            fi
        fi
    done < "$LIST_FILE"

    rm -r "$WORK_DIR"
else
    echo "Cannot create work directory $WORK_DIR"
    exit 2
fi
