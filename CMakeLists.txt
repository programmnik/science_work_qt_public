cmake_minimum_required(VERSION 3.16)
project(science_work VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Qt с компонентами для OpenGL
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets OpenGL)

# Для Qt6 нужен дополнительный модуль OpenGLWidgets
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)
endif()

# Исходные файлы
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    GLWidget.cpp
    # app_icon.rc
)

set(PROJECT_HEADERS
    mainwindow.h
    GLWidget.h
)

set(PROJECT_FORMS
    mainwindow.ui
)


set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")

# Создание исполняемого файла
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(science_work
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_FORMS}
        point.h point.cpp
        cameracore.h cameracore.cpp
        plane.h plane.cpp
        constants.h
        sphere.h sphere.cpp
        projectpanel.h projectpanel.cpp
        resources.qrc
        science_work.desktop
    )
else()
    add_executable(science_work
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_FORMS}
    )
endif()

# Линковка библиотек
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(science_work PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    )
else()
    target_link_libraries(science_work PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
    )
endif()

# Для Windows добавляем OpenGL
if(WIN32 AND ${QT_VERSION_MAJOR} LESS 6)
    target_link_libraries(science_work PRIVATE opengl32)
endif()

# Настройки для macOS и iOS
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

# Установка
include(GNUInstallDirs)
install(TARGETS science_work
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Финализация для Qt6
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(science_work)
endif()

# Установка иконки (она у вас уже есть, проверьте путь)
install(FILES "images/Riemann.png"
        DESTINATION "share/icons/hicolor/256x256/apps"
        RENAME "${PROJECT_NAME}.png")

# Установка .desktop файла
install(FILES "science_work.desktop"
        DESTINATION "share/applications")

