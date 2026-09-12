add_library(cameraLibrary STATIC
    camera.cpp          camera.h
    cameracore2d.cpp    cameracore2d.h
    cameracore3d.cpp    cameracore3d.h
)

target_include_directories(cameraLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(cameraLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
