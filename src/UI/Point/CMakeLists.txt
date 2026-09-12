add_library(pointLibrary STATIC
    point.cpp    point.h
)

target_include_directories(pointLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
