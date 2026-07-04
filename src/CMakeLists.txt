add_library(ndshin_core STATIC
    raytracer.cpp
    ray.cpp
    sphere.cpp
    hyperbox.cpp
    ellipsoid.cpp
    hyperplane.h
    shape.h
    camera.cpp
    window.cpp
    scene.cpp
    light.h
)
target_include_directories(ndshin_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ndshin_core PUBLIC glm::glm glfw glad)

# OpenMP parallelizes the per-row render loop. Optional: without it the
# #pragma omp is ignored and the renderer still runs single-threaded.
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(ndshin_core PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(ndshin main.cpp)

target_link_libraries(ndshin PRIVATE ndshin_core)
