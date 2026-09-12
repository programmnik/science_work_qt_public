add_library(dataLibrary STATIC
    objectmanager.cpp  objectmanager.h
    sceneobject.cpp    sceneobject.h
)

target_include_directories(dataLibrary PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(dataLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
