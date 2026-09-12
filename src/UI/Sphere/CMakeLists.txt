add_library(sphereLibrary STATIC
    sphere.cpp          sphere.h
    spherepoint.cpp     spherepoint.h
)

target_include_directories(sphereLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/UI
)

target_link_libraries(sphereLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
