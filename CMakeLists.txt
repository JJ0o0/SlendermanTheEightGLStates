cmake_minimum_required(VERSION 3.20)
project(
    SlendermanTEGS
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${CMAKE_PROJECT_NAME}
    "src/main.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        "./include"
        "./external"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        glfw
)