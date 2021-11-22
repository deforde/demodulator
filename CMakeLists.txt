cmake_minimum_required(VERSION 3.0.0)
project(demodulator C)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)

add_executable(demodulator src/main.c src/fft.c src/fir_filter.c src/plot.c src/wav.c)

target_link_libraries(demodulator fftw3 m)
