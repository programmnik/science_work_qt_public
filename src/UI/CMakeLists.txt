add_library(uiLibrary INTERFACE)         # ← INTERFACE, не STATIC!
target_link_libraries(uiLibrary INTERFACE
    cameraLibrary
    planeLibrary
    pointLibrary
    sphereLibrary
    toggleButtonLibrary
)

add_subdirectory(Camera)
add_subdirectory(Plane)
add_subdirectory(Point)
add_subdirectory(Sphere)
add_subdirectory(ToggleButton)
