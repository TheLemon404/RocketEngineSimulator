set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")

set(ASSIMP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/dependencies/assimp/build/include ${PROJECT_SOURCE_DIR}/dependencies/assimp/include)
set(ASSIMP_LIBRARIES ${PROJECT_SOURCE_DIR}/dependencies/assimp/build/lib/libassimp.dll.a)

file(GLOB_RECURSE src CONFIGURE_DEPENDS "*.cpp" "*.h" ${PROJECT_SOURCE_DIR}/dependencies/stbi/stb_image.h)
add_executable(RES ${src})

target_include_directories(RES PRIVATE ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(RES PRIVATE glfw glm glad ImGui ImGuiBackends ImGuizmo ImOGuizmo ${ASSIMP_LIBRARIES})