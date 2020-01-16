cmake_minimum_required (VERSION 2.8)

project ( viewer CXX)

set ( VIEWER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set ( VIEWER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( /opt/local/include )

set ( VIEWER_SOURCES
    src/viewer.cpp
)

set ( VIEWER_HEADERS
)

find_package( glm REQUIRED )

set (VIEWER_LIBS 
    GL
    GLEW
    glut
    glm
)

add_executable ( ${PROJECT_NAME} ${VIEWER_SOURCES} )
target_link_libraries ( ${PROJECT_NAME} ${VIEWER_LIBS} )