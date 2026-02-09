#!/usr/bin/env bash

set -e

cmake -S . -B build
cmake --build build
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./build/MyBot" "./build/MyBot"
