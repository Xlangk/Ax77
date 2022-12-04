project(Ax77Emulator)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE Files "Source/*.h" "Source/*.cpp")
add_executable(server Source/Server.cpp ${Files})

# Add X11
find_package(X11 REQUIRED)
target_link_libraries(server ${X11_LIBRARIES})
target_include_directories(server PRIVATE ${X11_INCLUDE_DIR})
