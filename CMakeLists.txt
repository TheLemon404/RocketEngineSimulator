cmake_minimum_required(VERSION 3.31)
project(RES)

set(CMAKE_CXX_STANDARD 20)

#dependencies
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/imgui)
add_subdirectory(dependencies/imoguizmo)
add_subdirectory(dependencies/imguizmo)

add_subdirectory(source)

#copy shaders to binary folder
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")
