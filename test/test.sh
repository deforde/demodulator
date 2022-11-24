#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

SCRIPT_DIR=$(realpath ${0%/*})

cd $SCRIPT_DIR/..
make

cd test

if [[ ! -d rds ]]; then
  curl -LO https://sdrplay.com/resources/IQ/rds.zip
  unzip rds.zip
fi

../build/./demodulator -f rds/*.wav -m fm

# ffplay -f s16le -ar 44100 -ac 1 mono_audio.bin
ffplay -f s16le -ar 44100 -ac 2 stereo_audio.bin
