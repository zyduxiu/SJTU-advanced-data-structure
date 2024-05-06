cmake_minimum_required(VERSION 3.24)
project(lsmkv)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(lsmkv
        kvstore.cc
        kvstore.h
        skiplist.cc
        skiplist.h main.cpp SSTable.cpp SSTable.h datastructure.h Bloomfilter.cpp Bloomfilter.h vlog.h vlog.cpp utils.h memtable.cpp memtable.h)
