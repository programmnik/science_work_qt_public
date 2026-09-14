cmake_minimum_required(VERSION 3.16)
project(science_work VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# Установка
include(GNUInstallDirs)

# Установка иконки
install(FILES "images/Riemann.png"
        DESTINATION "share/icons/hicolor/256x256/apps"
        RENAME "${PROJECT_NAME}.png")

# Установка .desktop файла
install(FILES "science_work.desktop"
        DESTINATION "share/applications")
