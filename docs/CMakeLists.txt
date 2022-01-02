find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

#--------------------------------------------------------

# Find all the public headers.
file(GLOB_RECURSE MOSAIC_PUBLIC_HEADERS ${mosaic_SOURCE_DIR}/include/mosaic/*.h)

set(DOXYGEN_INPUT_DIR ${mosaic_SOURCE_DIR}/Mosaic)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_EXCLUDE_DIR "")
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})


add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${MOSAIC_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs"
                   VERBATIM
                   EXCLUDE_FROM_ALL)

add_custom_target(Doxygen DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND 
                       ${SPHINX_EXECUTABLE} -b html 
                       # Tell Breathe where to find the Doxygen output
                       -Dbreathe_projects.Mosaic=${DOXYGEN_OUTPUT_DIR}/xml 
                       ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   DEPENDS
                       # Other doc files you want to track should go here (or in some variable)
                       ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                       ${DOXYGEN_INDEX_FILE}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx"
                   VERBATIM
                   EXCLUDE_FROM_ALL)

add_custom_target(Sphinx DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
include(GNUInstallDirs)
install(DIRECTORY ${SPHINX_BUILD}
        DESTINATION ${CMAKE_INSTALL_DOCDIR})