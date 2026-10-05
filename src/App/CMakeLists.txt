if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(science_work MANUAL_FINALIZATION
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ../../resources.qrc
        customtitlebar.h customtitlebar.cpp
        custommenubar.h custommenubar.cpp
    )
else()
    add_executable(science_work
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ../../resources.qrc
    )
endif()

target_include_directories(science_work PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(science_work PRIVATE
    panelLibrary
)
