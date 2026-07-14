#!/bin/bash

set -e
cmake --build build

rsync -a assets/ build/assets/