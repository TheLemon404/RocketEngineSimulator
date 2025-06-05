set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "*.cpp" "*.h")
add_executable(RES ${src})

target_link_libraries(RES PRIVATE glfw glm glad ImGui ImGuizmo ImOGuizmo)