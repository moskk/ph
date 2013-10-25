cmake_minimum_required(VERSION 2.6)

project(ph)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (SOURCE 
  ph.cpp
  crypt.cpp
  utils.cpp)

set (HEADER ph.hpp crypt.h utils.h)

add_executable (ph ${SOURCE})
