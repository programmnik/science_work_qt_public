add_library(openGLLibrary STATIC
    GLWidget.cpp GLWidget.h
    skyrenderer.h skyrenderer.cpp
)

target_include_directories(openGLLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(openGLLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    uiLibrary
)
