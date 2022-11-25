project(Ax77Emulator)
cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE Files "src/*.hh")
add_executable(server src/server.cc ${Files})
