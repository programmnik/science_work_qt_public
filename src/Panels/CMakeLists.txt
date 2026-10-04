add_library(panelLibrary STATIC
    ObjectPanel/objectspanelwidget.cpp  ObjectPanel/objectspanelwidget.h
    ProjectPanel/projectpanel.cpp       ProjectPanel/projectpanel.h
    PreviewPanel/previewpanel.cpp       PreviewPanel/previewpanel.h
    SettingsPanel/settingspanel.h       SettingsPanel/settingspanel.cpp
    MenuPanel/menupanel.h MenuPanel/menupanel.cpp
    ProjectPanel/mathpanel.h ProjectPanel/mathpanel.cpp
)

target_include_directories(panelLibrary PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(panelLibrary PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    coreLibrary
)
