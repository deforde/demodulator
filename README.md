# demodulator
Implementations of various demodulation techniques.

[![build](https://github.com/deforde/demodulator/actions/workflows/build.yml/badge.svg)](https://github.com/deforde/demodulator/actions/workflows/build.yml)

Takes in source data via `.wav` files, demodulates (and, if applicable, filters, downsamples, decodes etc.) the data contained within, and writes the output to file.

-----------------------------------------------------------

## Building
To install the required dependencies (locally, not system wide) and build `demodulator` simply run the `build.sh` script.
To just build the `demodulator`, run `make`.

-----------------------------------------------------------

## Dependecies
- [fftw](https://www.fftw.org/)
- [liquid-dsp](https://github.com/jgaeddert/liquid-dsp)

To download, build and install (locally) the required dependencies, simply run the `build.sh` script.

Alternatively, check out the workflow script here: `.github/workflows/build.yml`, to get an idea of how to go about installing the necessary dependencies manually, should you so wish.

-----------------------------------------------------------

## Testing
In the test directory is a script that will build `demodulator`, download a test file, run it through `demodulator` and playback the raw audio output.
These tests depend on `ffplay`, which is contained in the `ffmpeg` [package](https://manpages.ubuntu.com/manpages/jammy/man1/ffplay.1.html).

-----------------------------------------------------------

### TODO
- Improve RDS decoding (currently only extracts data from offset blocks `A` and `B`).
- Write RDS data to file (instead of just to `stdout`).
- Implement demodulation of other modulation types (some great source data files are available [here](https://www.sdrplay.com/iq-demo-files/)).
