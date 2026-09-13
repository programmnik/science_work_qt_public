add_library(coreLibrary STATIC
    constants.h
    fpscontroller.cpp   fpscontroller.h
    raycaster.cpp       raycaster.h
    complexnumber.cpp   complexnumber.h
    bigfloat.h          bigfloat.cpp
    zoomcontroller.h    zoomcontroller.cpp
)

target_include_directories(coreLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(coreLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
