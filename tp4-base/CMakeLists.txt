# CMakeList.txt : CMake project for tp4_server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("tp4_base")

# Add source to this project's executable.
add_executable ("src/server/main.cpp")
add_executable ("src/client/main.cpp")

# TODO: Add tests and install targets if needed.
