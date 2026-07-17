add_subdirectory(fastgltf)

# IMGUI
add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp

    imgui/misc/cpp/imgui_stdlib.cpp

    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
)

target_link_libraries(imgui
    PUBLIC
        glfw
)

# INIH

add_library(inih
    inih/ini.c
    inih/cpp/INIReader.cpp
)

target_include_directories(inih PUBLIC
    inih
    inih/cpp
)