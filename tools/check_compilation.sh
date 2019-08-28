#!/bin/sh

if [ "$1" = "" ]; then
    echo "Script compiles all demos"
    echo "Usage: check_compilation.sh [platform]"
    echo "        platform   actual platform name or all"
    exit 1
fi

platforms=$1
if [ "$1" = "all" ]; then
    platforms="arduino avr linux"
fi

# set -x

for platform in $platforms
do

    rm -rf ../bld
    for topdir in `cd ../examples && ls -d */*/`
    do
        example=${topdir%?}
        ./build_and_run.sh -p $platform $2 $example
        if [ $? -ne 0 ]; then
            echo "ERROR"
            exit 1
        fi
    done

done

