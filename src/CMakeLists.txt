add_library(ndshin_core STATIC
    raytracer.cpp
    ray.cpp
    sphere.cpp
    shape.h
    camera.cpp
    window.cpp
    scene.cpp
)
target_include_directories(ndshin_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ndshin_core PUBLIC glm::glm glfw glad)

add_executable(ndshin main.cpp)

target_link_libraries(ndshin PRIVATE ndshin_core)
