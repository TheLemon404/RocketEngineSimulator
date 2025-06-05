//
// Created by Osprey on 6/4/2025.
//

#pragma once

#ifndef GRAPHICS_OBJECTS_H
#define GRAPHICS_OBJECTS_H

#include <string>
#include <vector>

#endif //GRAPHICS_OBJECTS_H
struct Model {
    unsigned int id;

    Model() { id = rand(); };

    std::vector<float> vertices;
    std::vector<int> indices;
};

struct ShaderObject {
    unsigned int id;

    ShaderObject(int type);
    void Load(std::string localPath);
    ~ShaderObject();
};

struct ShaderProgramObject {
    unsigned int id;
    ShaderObject* vertexShaderObject;
    ShaderObject* fragmentShaderObject;

    ShaderProgramObject();
    void Compile(ShaderObject* vertexShaderObject, ShaderObject* fragmentShaderObject);
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
struct  BufferObject {
    BufferObject();
    virtual ~BufferObject();

    unsigned int id;

    void Upload(std::vector<T> data);
    void Bind();
    void Unbind();
};