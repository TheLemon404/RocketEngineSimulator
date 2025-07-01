cmake_minimum_required(VERSION 3.31)
project(RES)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dependencies/glfw)

# Fix GLFW MinGW issues
target_compile_definitions(glfw PRIVATE
        WIN32_LEAN_AND_MEAN
        MINIMAX
        _WIN32_WINNT=0x0600
)

if(!WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    find_package(assimp REQUIRED)
endif ()

add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/imgui)
add_subdirectory(dependencies/imoguizmo)
add_subdirectory(dependencies/imguizmo)

add_subdirectory(source)

#copy shaders to binary folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

#if we are running on linux, do not copy the assimp dll to the build directory. We will be using a pacman package
if(WIN32)
    file(COPY ${PROJECT_SOURCE_DIR}/dependencies/assimp/build/bin/libassimp-5.dll DESTINATION ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
endif()