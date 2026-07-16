cmake_minimum_required(VERSION 3.20)
project(
    SlendermanTEGS
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(external/fastgltf)

add_executable(${CMAKE_PROJECT_NAME}
    "src/main.cpp"
    "src/game.cpp"
    "src/world.cpp"
    "src/application.cpp"
    
    "src/graphics/shader.cpp"
    "src/graphics/mesh.cpp"
    "src/graphics/model_loader.cpp"
    "src/graphics/texture.cpp"
    "src/graphics/cubemap.cpp"
    "src/graphics/framebuffer.cpp"
    "src/graphics/skeleton.cpp"
    "src/graphics/shadow_map.cpp"
    "src/graphics/shadow_renderer.cpp"
    "src/graphics/skybox_mesh.cpp"
    "src/graphics/skybox_renderer.cpp"
    "src/graphics/game_renderer.cpp"
    "src/graphics/animation/animation_clip.cpp"
    "src/graphics/animation/animator.cpp"
    "src/graphics/materials/basic_material.cpp"
    "src/graphics/materials/pbr_material.cpp"

    "src/platform/window.cpp"
    "src/platform/error_handling.cpp"

    "src/playable/camera.cpp"
    "src/playable/flashlight.cpp"
    "src/playable/player.cpp"

    "src/external/tinyfiledialogs.c"
    "src/external/gl.c"
    "src/external/stb_image.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        "./include"
        "./external"
)

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME}
        comdlg32
        ole32
        user32
        shell32
    )

    target_link_options(${CMAKE_PROJECT_NAME}
        PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        OpenGL::GL
        glfw
        glm::glm
        fastgltf::fastgltf
)