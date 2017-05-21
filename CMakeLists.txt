cmake_minimum_required(VERSION 3.7)
project(Lab4)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp Ipc.cpp Ipc.h Dh.cpp Dh.h blowfish.cpp blowfish.h bignum.cpp bignum.h bn_mul.h config.h)

add_executable(Lab4_server ${SOURCE_FILES})
target_compile_definitions(Lab4_server PRIVATE SERVER)

add_executable(Lab4_client ${SOURCE_FILES})
target_compile_definitions(Lab4_client PRIVATE CLIENT)