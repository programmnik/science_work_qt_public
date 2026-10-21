add_library(localeLibrary STATIC
    LocaleStore.h  LocaleStore.cpp
    locale.qrc
)

target_include_directories(localeLibrary PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(localeLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)
