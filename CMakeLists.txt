cmake_minimum_required(VERSION 3.8.2)
project(Ragnarok C)

set(CMAKE_C_STANDARD 90)

add_executable(Ragnarok ragnarok.c
        src/info.c
        include/info.h
        include/search.h
        src/search.c
        makefile)

target_link_libraries(Ragnarok m)