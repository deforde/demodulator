#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

SCRIPT_DIR=$(realpath ${0%/*})
cd $SCRIPT_DIR

if [[ ! -d fftw ]]; then
  mkdir -p fftw/src
  curl -L https://www.fftw.org/fftw-3.3.10.tar.gz | \
  tar --strip-components=1 -xz -C fftw/src
  cd fftw/src
  PREFIX=$(realpath ..)
  ./configure --prefix $PREFIX --enable-float
  make
  make install
fi

if [[ ! -d liquid-dsp ]]; then
  mkdir liquid-dsp
  cd liquid-dsp
  git clone https://github.com/jgaeddert/liquid-dsp src
  cd src
  PREFIX=$(realpath ..)
  ./bootstrap.sh
  ./configure --prefix $PREFIX
  make
  make install
fi

make
