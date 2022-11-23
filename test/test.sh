#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

SCRIPT_DIR=$(realpath ${0%/*})

cd $SCRIPT_DIR/..
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .

cd ../test

if [[ ! -d rds ]]; then
  curl -LO https://sdrplay.com/resources/IQ/rds.zip
  unzip rds.zip
fi

../build/./demodulator -f rds/*.wav -m fm

ffplay -f s16le -ar 44k -ac 2 stereo_audio.bin
