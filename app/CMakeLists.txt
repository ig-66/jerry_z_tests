cmake_minimum_required(VERSION 3.22)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(jerry_z_tests)

add_subdirectory(src)