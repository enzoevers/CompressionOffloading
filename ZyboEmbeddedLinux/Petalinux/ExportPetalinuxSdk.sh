#!/bin/sh

SDK_SYSROOT_PATH=$1
SDK_EXPORT_PATH=$2

if [ -d ${SDK_EXPORT_PATH} ]; then
    rm -r ${SDK_EXPORT_PATH}
fi
mkdir -p ${SDK_EXPORT_PATH}

cp -r --dereference ${SDK_SYSROOT_PATH}/usr ${SDK_EXPORT_PATH}
cp -r --dereference ${SDK_SYSROOT_PATH}/lib ${SDK_EXPORT_PATH}

# Windows is case insesitive. This requires some extra filtering.
# The lower-case files are of interest.
# 1. Sort with all characters as upper case.
# 2. Print sorted paths in reserve order, putting upper case file on top.
# 3. Get the first item from each diplicate group, ignoring case. Leading to only showing the upper case files.
# 4. Append '.duplicate' to the upper case duplicate files
find ${SDK_EXPORT_PATH} \
    | sort -f \
    | tac \
    | uniq -di \
    | xargs -I {} mv {} {}.duplicate