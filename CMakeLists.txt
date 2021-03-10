cmake_minimum_required(VERSION 3.10)

# set the project name
set(CMAKE_BUILD_TYPE Debug)
project(neovim-irc VERSION 1.0)

set(PROJECT_LIB_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

