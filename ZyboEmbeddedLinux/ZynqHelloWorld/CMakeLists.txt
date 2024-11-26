cmake_minimum_required(VERSION 3.05)

SET(CMAKE_C_FLAGS "-g -Wall -Wextra -O0 -g3")

project(ZynqHelloWorld LANGUAGES C VERSION 1.0)

add_executable(${CMAKE_PROJECT_NAME} main.c)
