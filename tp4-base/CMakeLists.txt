# CMakeList.txt : CMake project for tp4_server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project ("tp4_base")

# Add source to this project's executable.
add_executable (server "src/server/main.cpp")
add_executable (client "src/client/main.cpp")

# add_compile_options(-pthread)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# TODO: Add tests and install targets if needed.
