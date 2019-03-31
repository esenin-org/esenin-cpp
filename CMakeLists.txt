cmake_minimum_required (VERSION 3.0)
project (Esenin)

add_executable(Esenin src/esenin.cxx)

find_library(LIB_CURL curl)

target_link_libraries(Esenin "${LIB_CURL}")

target_include_directories(Esenin PRIVATE include)

install(TARGETS Esenin DESTINATION bin)