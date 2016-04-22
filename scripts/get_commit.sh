#!/bin/sh

# Script used to get commit hash
#
# Usage 'get_commit.sh [directory_of_CMakeLists.txt]'
#
# Copyright (C) 2016 Arthur M

src_root="$(pwd)"

if [ -n "$1" ]; then
  src_root=$1
fi

cd $src_root

if [ -d ".git" ]; then
  commit=$(git log --oneline -n 1 | cut -c 1-7)
  echo -n $commit
else
  echo "0000000"
fi
