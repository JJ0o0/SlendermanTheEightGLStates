#!/bin/bash

set -e
cmake --build build

rm -rf build/assets
rsync -a assets/ build/assets/