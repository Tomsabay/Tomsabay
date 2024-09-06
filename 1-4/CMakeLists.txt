cmake_minimum_required(VERSION 3.10)
project(HelloCMake VERSION 3.1)
set(CMAKE_C_STANDARD 99)

add_library(my_static_lib STATIC main.c)

add_library(my_dynamic_lib SHARED main.c)

add_executable(b main.c)

install(TARGETS my_static_lib my_dynamic_lib
	LIBRARY DESTINATION dir
	ARCHIVE DESTINATION dir)
