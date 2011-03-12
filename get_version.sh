#!/bin/bash

cd "$(dirname "$0")"

cat version.h | grep CGVERSION | sed "s/^.*\"\(.*\)\".*$/\1/g"
