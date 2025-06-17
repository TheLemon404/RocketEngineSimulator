//
// Created by Osprey on 6/4/2025.
//

#pragma once

#ifndef GRAPHICS_OBJECTS_H
#define GRAPHICS_OBJECTS_H

#include <string>
#include <vector>

#include "../simulation/gas_simulation.h"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/ext/matrix_transform.hpp"

#endif //GRAPHICS_OBJECTS_H

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
    void CleanUp();
};

template<typename T>
struct BufferObject {
    BufferObject();
    virtual ~BufferObject();

    unsigned int id;

    void Upload(std::vector<T> data);
    void Bind();
    void Unbind();
    void CleanUp();
};

struct Control {
    glm::vec3 position;
    float bevelRadius = 0.1f;
    bool selected = false;
    int bevelNumber = 0;

    Control(glm::vec3 position) : position(position) {};
    int GetNumBeveledVertices();

private:
    std::vector<glm::vec3> RecursiveBevel(glm::vec3 a, glm::vec3 v, glm::vec3 b, float distance, int depth = 1);

public:

    void CheckSelection(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution, float radius);
    std::vector<glm::vec3> ExtractBeveledPositions(glm::vec3 prevPoint, glm::vec3 nextPoint);
};

struct LinePath {
    unsigned int id = rand();
    std::vector<Control> controls;

    static glm::vec3 RoundToMajorAxis(const glm::vec3& v);
    static glm::vec3 RountToMajorPlane(const glm::vec3& v);

    LinePath(std::vector<Control> controls) : controls(controls) {};

    void UpdatePositionsArray();
    std::vector<glm::vec3> ExtractPositions();
    int GetSelectedControlIndex(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution, float radius);

    int Extrude(int controlIndex, glm::vec3 to);
    void Delete(int controlIndex);
    void UpdatePositionsBuffer();

    int GetNumVertices();

    float GetLineLength();

    std::vector<float> positions;

    VertexArrayObject* vao;
    BufferObject<float>* positionsBuffer;
};

struct Pipe {
    unsigned int id = rand();
    LinePath path;

    //rendering
    int segments = 32;
    float radius = 0.2f;
    glm::vec3 color = glm::vec3(1.0f, 0.8f, 0.5f);

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<unsigned int> indices;

    VertexArrayObject* vao;
    BufferObject<float>* positionsBuffer;
    BufferObject<float>* normalsBuffer;
    BufferObject<unsigned int>* indicesBuffer;

    //flow physics
    float massFlowRate;

    Pipe(LinePath path) : path(path) {};

    void UpdatePositionsBuffer();

    void ComputeMassFlowRate(float density, float velocity);

    void TransportFrame(glm::vec3 prevTangent, glm::vec3 newTangent, glm::vec3& right, glm::vec3& up);
    std::vector<glm::vec3> GenerateRingWithFrame(glm::vec3 center, glm::vec3 tangent, glm::vec3 right, glm::vec3 up, float radius);
    void UpdateArrays();
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

struct Material {
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct Mesh {
    unsigned int id = rand();

    Material material = {};

    void UpdateBuffers();

    std::vector<float> vertices = {};
    std::vector<float> uvs = {};
    std::vector<float> normals = {};
    std::vector<unsigned int> indices = {};

    VertexArrayObject* vao;
    BufferObject<float>* positionsBuffer;
    BufferObject<float>* uvsBuffer;
    BufferObject<float>* normalsBuffer;
    BufferObject<unsigned int>* indicesBuffer;
};

struct Model {
    unsigned int id = rand();

    static Model LoadModelFromOBJ(std::string localPath);

    std::vector<Mesh> meshes;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    Model(std::vector<Mesh> meshes) : meshes(meshes) {};
};

struct Scene {
    std::vector<Model> models;
    std::vector<Pipe> pipes;
    Camera camera;

    void CleanUp();
};
