cmake_minimum_required (VERSION 3.0)
project (esenin VERSION 0.1.0 DESCRIPTION "C++ wrapper for Esenin Server")
include(GNUInstallDirs)
add_library(esenin SHARED src/esenin.cxx)
set_target_properties(esenin PROPERTIES 
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER include/esenin/esenin.h)
find_library(LIB_CURL curl)
target_link_libraries(esenin "${LIB_CURL}")
target_include_directories(esenin PRIVATE include)
install(TARGETS esenin
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include)