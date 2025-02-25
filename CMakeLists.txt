cmake_minimum_required(VERSION 3.29)
project(PSS C)

set(CMAKE_C_STANDARD 11)

add_executable(Generate
        person.h
        generator.c)
add_executable(Write
        main.c
        person.h)



include_directories(/usr/local/include)


set(CMAKE_PREFIX_PATH "/usr/local/x86_64-w64-mingw32")  # Adjust the path to your PCRE installation location
