cmake_minimum_required(VERSION 3.14)

project(
    mosaic 
    VERSION 0.1.0 
    LANGUAGES CXX
    )


set(CMAKE_MODULE_PATH "${mosaic_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(docs)
