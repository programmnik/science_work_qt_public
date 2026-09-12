add_library(planeLibrary STATIC
    plane.cpp          plane.h
    plane3d.cpp        plane3d.h
    plane2d.cpp        plane2d.h
)

target_include_directories(planeLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/UI
)

target_link_libraries(planeLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
