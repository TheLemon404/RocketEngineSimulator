//
// Created by Osprey on 6/4/2025.
//

#pragma once

#ifndef GRAPHICS_OBJECTS_H
#define GRAPHICS_OBJECTS_H

#include <string>
#include <vector>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/ext/matrix_transform.hpp"

#endif //GRAPHICS_OBJECTS_H

enum MeshRenderMode {
    NORMAL,
    UNLIT
};

struct Mesh;

enum CameraProjectionMode {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct Camera {
    float fov = 70.0f;
    float zoomFactor = 1.0f;
    CameraProjectionMode projectionMode = PERSPECTIVE;
    glm::vec3 position = glm::vec3(2.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    void RotateAround(float angle, glm::vec3 axis, glm::vec3 originPoint);
};

struct ShaderObject {
    unsigned int id;

    ShaderObject(int type);
    void Load(std::string localPath);
    ~ShaderObject();
};

struct ShaderProgramObject {
    unsigned int id;
    ShaderObject* vertexShaderObject = nullptr;
    ShaderObject* fragmentShaderObject = nullptr;
    ShaderObject* tesselationControlShaderObject = nullptr;
    ShaderObject* tesselationEvaluationShaderObject = nullptr;

    ShaderProgramObject();

    void UploadUniformFloat(const char* uniformName, float value);
    void UploadUniformInt(const char* uniformName, int value);
    void UploadUniformVec2(const char* uniformName, glm::vec2 value);
    void UploadUniformVec3(const char* uniformName, glm::vec3 value);
    void UploadUniformVec4(const char* uniformName, glm::vec4 value);
    void UploadUniformMat4(const char* uniformName, glm::mat4 value);

    void Compile(ShaderObject* vertexShaderObject, ShaderObject* fragmentShaderObject);
    void CompileTesselation(ShaderObject* vertexShader, ShaderObject* controlShader, ShaderObject* evaluationShader, ShaderObject* fragmentShader);
    void Use();
    ~ShaderProgramObject();
};

struct VertexArrayObject {
    unsigned int id;

    VertexArrayObject();

    void CreateVertexAttributePointer(int location, int length, int size, int type);
    void Bind();
    void Unbind();
};

template<typename T>
struct BufferObject {
    BufferObject();
    virtual ~BufferObject();

    unsigned int id;

    void Upload(std::vector<T> data);
    void Bind();
    void Unbind();
};

struct SelectableSpace {
    glm::vec3 position;
    float radius = 0.2f;
    bool selected = false;

    SelectableSpace(glm::vec3 position) : position(position) {};

    void CheckSelection(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution);
};

struct Spline {
    unsigned int id = rand();
    SelectableSpace p0, p1, p2, p3;

    Spline(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}

    std::vector<float> ExtractPositions();
    SelectableSpace* GetSelectedGizmo(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution);

    void UpdatePositionsBuffer();

    VertexArrayObject* vao;
    BufferObject<float>* positionsBuffer;
};

struct TextureObject {
    unsigned int id;
    unsigned int width, height;

    TextureObject(unsigned int width, unsigned int height);

    void Load(std::string localPath);
    void SetData(void* data);

    void Bind();
    void Unbind();
};

struct Mesh {
    unsigned int id = rand();

    MeshRenderMode renderMode = NORMAL;

    static Mesh LoadmeshFromOBJ(std::string localPath, int meshIndex = 0);

    void UpdateBuffers();

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    std::vector<float> vertices = {};
    std::vector<float> uvs = {};
    std::vector<float> normals = {};
    std::vector<int> indices = {};

    VertexArrayObject* vao;
    BufferObject<float>* positionsBuffer;
    BufferObject<float>* uvsBuffer;
    BufferObject<float>* normalsBuffer;
    BufferObject<int>* indicesBuffer;
};

struct Scene {
    std::vector<Mesh> meshes;
    std::vector<Spline> splines;
    Camera camera;

    Mesh* GetMeshFromID(unsigned int id) { for (int i = 0; i < meshes.size(); i++) if (meshes[i].id == id) return &meshes[i]; };
    Spline* GetSplineFromID(unsigned int id) { for (int i = 0; i < splines.size(); i++) if (splines[i].id == id) return &splines[i]; };
};
