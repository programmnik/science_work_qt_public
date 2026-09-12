add_library(toggleButtonLibrary STATIC
    togglebutton.cpp
    togglebutton.h
)

target_include_directories(toggleButtonLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(toggleButtonLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
