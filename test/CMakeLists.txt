cmake_minimum_required(VERSION 3.2)

set(This mosaicTests)

set(
    Sources
    src/mosaicTests.cpp
    )

add_executable(
    ${This}
    ${Sources}
    )

set_target_properties(
    ${This} 
    PROPERTIES
    FOLDER tests
    )

target_include_directories(
    ${This} 
    PRIVATE 
    ${mosaic_SOURCE_DIR}/include
)

add_test(
    NAME ${This}
    COMMAND ${This}
    )