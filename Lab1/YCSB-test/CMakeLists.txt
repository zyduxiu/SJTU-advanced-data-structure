cmake_minimum_required(VERSION 3.24)
project(lab1_test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(lab1_test
        compressed_radix_tree.cpp
        compressed_radix_tree.hpp
        main.cpp
        radix_tree.cpp
        radix_tree.hpp
        tree.hpp
        util.hpp rbtree.h rbtree.cpp test.cpp)
