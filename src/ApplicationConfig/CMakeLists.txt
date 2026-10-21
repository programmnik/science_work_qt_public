add_library(configLibrary STATIC
    AppConfig.h    AppConfig.cpp
    config.qrc
    ModuleConfig.h
)

target_include_directories(configLibrary PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(configLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)
