cmake_minimum_required(VERSION 3.13)
project(demodulator C)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(demodulator "")

add_subdirectory(src)
add_subdirectory(src/concurrency)
add_subdirectory(src/decode)
add_subdirectory(src/dsp)
add_subdirectory(src/fm)
add_subdirectory(src/interpret)
add_subdirectory(src/io)
add_subdirectory(src/types)

target_link_libraries(demodulator liquid fftw3f pthread m)
