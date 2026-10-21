add_library(directoryStoreLibrary STATIC
    DirectoryStore.h DirectoryStore.cpp
)

target_include_directories(directoryStoreLibrary PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(directoryStoreLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)
