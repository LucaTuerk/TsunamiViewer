cmake_minimum_required (VERSION 3.1)

project ( viewer CXX)

set ( VIEWER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set ( VIEWER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )
set ( CMAKE_CXX_STANDARD 17)

include_directories( /opt/local/include )

set ( VIEWER_SOURCES
    src/viewer.cpp
    src/data/netcdf/netcdfReader.cpp
)

set ( VIEWER_HEADERS
)

find_package( glm REQUIRED )

set (VIEWER_LIBS 
    GL
    GLEW
    glut
    glm
    netcdf
)

add_executable ( ${PROJECT_NAME} ${VIEWER_SOURCES} )
target_link_libraries ( ${PROJECT_NAME} ${VIEWER_LIBS} )