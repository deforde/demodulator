# demodulator
Implementations of various demodulation techniques.

[![build](https://github.com/deforde/demodulator/actions/workflows/build.yml/badge.svg)](https://github.com/deforde/demodulator/actions/workflows/build.yml)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/deforde/demodulator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/deforde/demodulator/context:cpp)

Takes in source data via `.wav` files, demodulates (and, if applicable, filters, downsamples, decodes etc.) the data contained within, and writes the output to file.

-----------------------------------------------------------

### TODO
- Take in demodulation type and source data file as command line arguments.
- Implement demodulation of RDS data in FM signals.
- Implement demodulation of other modulation types (some great source data files are available [here](https://www.sdrplay.com/iq-demo-files/)).
