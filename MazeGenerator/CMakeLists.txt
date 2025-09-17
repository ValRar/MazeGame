cmake_minimum_required(VERSION 3.8)

project(MazeGenerator)

add_library(MazeGenerator MazeGenerator.cpp)
target_include_directories(MazeGenerator PUBLIC  .)

