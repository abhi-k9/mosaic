set(This mosaicTests)

set(
    Sources
    src/mosaic_test.cpp
    src/typelist_test.cpp
    src/type_traits_test.cpp
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
    ${mosaic_SOURCE_DIR}/Mosaic/include
    ${mosaic_SOURCE_DIR}/deps/googletest/include
    )

target_link_libraries(
    ${This}
    PRIVATE
    ${mosaic_SOURCE_DIR}/deps/googletest/lib/libgtest.a
)

add_test(
    NAME ${This}
    COMMAND ${This}
    )