# demodulator
Implementations of various demodulation techniques.

[![build](https://github.com/deforde/demodulator/actions/workflows/build.yml/badge.svg)](https://github.com/deforde/demodulator/actions/workflows/build.yml)

Takes in source data via `.wav` files, demodulates (and, if applicable, filters, downsamples, decodes etc.) the data contained within, and writes the output to file.

-----------------------------------------------------------

## Building
To just build the `demodulator`, run `make`.
This will automatically download and build the required 3rd party dependencies if they do not already exist.

-----------------------------------------------------------

## Dependecies
- [fftw](https://www.fftw.org/)
- [liquid-dsp](https://github.com/jgaeddert/liquid-dsp)

These dependencies will be automatically downloaded and built when running `make`.

-----------------------------------------------------------

## Testing
In the test directory is a script that will build `demodulator`, download a test file, run it through `demodulator` and playback the raw audio output.
These tests depend on `ffplay`, which is contained in the `ffmpeg` [package](https://manpages.ubuntu.com/manpages/jammy/man1/ffplay.1.html).

-----------------------------------------------------------

### TODO
- Improve RDS decoding (currently only extracts data from offset blocks `A` and `B`).
- Write RDS data to file (instead of just to `stdout`).
- Implement demodulation of other modulation types (some great source data files are available [here](https://www.sdrplay.com/iq-demo-files/)).
