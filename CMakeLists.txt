cmake_minimum_required(VERSION 3.26)

project(Repomngr)

set(CMAKE_C_STANDARD 11)

file(GLOB SOURCES "*.c")
add_executable(${PROJECT_NAME} ${SOURCES})