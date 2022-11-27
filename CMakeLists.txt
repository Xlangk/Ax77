project(Ax77Emulator)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE Files "src/*.hh" "src/*.c")
add_executable(server src/server.cc ${Files})
