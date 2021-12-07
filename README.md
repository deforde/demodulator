# demodulator
Implementations of various demodulation techniques.

[![build](https://github.com/deforde/demodulator/actions/workflows/build.yml/badge.svg)](https://github.com/deforde/demodulator/actions/workflows/build.yml)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/deforde/demodulator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/deforde/demodulator/context:cpp)

Takes in source data via `.wav` files, demodulates (and, if applicable, filters, downsamples, decodes etc.) the data contained within, and writes the output to file.

-----------------------------------------------------------

## Building
Pretty much what you'd expect:
```
git clone https://github.com/deforde/demodulator
cd demodulator && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target demodulator
```

-----------------------------------------------------------

## Dependecies
- [fftw](https://www.fftw.org/)
- [liquid-dsp](https://github.com/jgaeddert/liquid-dsp)

Check out the workflow script here: `.github/workflows/build.yml`, to get an idea of how to go about installing the necessary dependencies.

-----------------------------------------------------------

### TODO
- Take in demodulation type and source data file as command line arguments.
- Improve RDS decoding (currently only extracts data from offset blocks `A` and `B`).
- Write RDS data to file (instead of just to `stdout`).
- Implement demodulation of other modulation types (some great source data files are available [here](https://www.sdrplay.com/iq-demo-files/)).
