add_library(themeLibrary STATIC
    dark/standarddark.h
)

target_include_directories(themeLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
