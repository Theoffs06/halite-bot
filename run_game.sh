#!/usr/bin/env bash

set -e

cmake -B build
cd build
make
cd .
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./build/MyBot" "./build/MyBot"
