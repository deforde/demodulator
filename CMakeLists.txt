cmake_minimum_required(VERSION 3.13)
project(demodulator C)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)

add_executable(demodulator "")

add_subdirectory(src)
add_subdirectory(src/fm)

target_link_libraries(demodulator fftw3 pthread m)
