cmake_minimum_required(VERSION 3.0.0)
project(digital-image-processing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(interpolation)
add_subdirectory(intensity-transformation)
add_subdirectory(affine-transformation)
add_subdirectory(histogram-equalization)
add_subdirectory(histogram-matching)
add_subdirectory(utility)