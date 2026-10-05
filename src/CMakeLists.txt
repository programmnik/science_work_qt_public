# Поиск OpenGL
find_package(OpenGL REQUIRED)

# Поиск Qt с компонентами для OpenGL и Multimedia
find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS Core Widgets OpenGL Multimedia MultimediaWidgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS Core Widgets OpenGL Multimedia MultimediaWidgets Svg)

# Для Qt6 нужен дополнительный модуль OpenGLWidgets
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)
endif()

add_subdirectory(App)
add_subdirectory(Core)
add_subdirectory(Data)
add_subdirectory(OpenGL)
add_subdirectory(Themes)
add_subdirectory(UI)
add_subdirectory(Panels)

# Линковка библиотек
target_link_libraries(science_work PRIVATE
    panelLibrary
    openGLLibrary
    dataLibrary
    uiLibrary
    cameraLibrary
    planeLibrary
    pointLibrary
    sphereLibrary
    toggleButtonLibrary
    coreLibrary
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Svg
    ${OPENGL_LIBRARIES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(science_work PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

if(WIN32)
    target_link_libraries(science_work PRIVATE opengl32)
    if(${QT_VERSION_MAJOR} LESS 6)
        target_link_libraries(science_work PRIVATE glu32)
    endif()
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(science_work PRIVATE GLU)
endif()

if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(GLUT_LIBRARY GLUT)
    target_link_libraries(science_work PRIVATE ${OPENGL_LIBRARY} ${GLUT_LIBRARY})
endif()

# Свойства цели
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.science_work)
endif()

set_target_properties(science_work PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Include от корня src/, чтобы писать #include "App/mainwindow.h" ---
target_include_directories(science_work PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(science_work PRIVATE panelLibrary)

include(GNUInstallDirs)
install(TARGETS science_work
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(science_work)
endif()
