//
// Created by Osprey on 6/4/2025.
//

#include "graphics_objects.h"

#include <stdexcept>

#include "../core/io.h"
#include "glad/glad.h"

ShaderObject::ShaderObject(int type) {
    id = glCreateShader(type);
}

void ShaderObject::Load(std::string localPath) {
    const char* source = IO::ReadFileGLSL(localPath);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        throw std::runtime_error(infoLog);
    }
}

ShaderObject::~ShaderObject() {
    glDeleteShader(id);
}

ShaderProgramObject::ShaderProgramObject() {
    id = glCreateProgram();
}

void ShaderProgramObject::Compile(ShaderObject* vertexShader, ShaderObject* fragmentShader) {
    vertexShaderObject = vertexShader;
    fragmentShaderObject = fragmentShader;

    glAttachShader(id, vertexShader->id);
    glAttachShader(id, fragmentShader->id);
    glLinkProgram(id);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        throw std::runtime_error(infoLog);
    }
}

void ShaderProgramObject::Use() {
    glUseProgram(id);
}

ShaderProgramObject::~ShaderProgramObject() {
    delete vertexShaderObject;
    delete fragmentShaderObject;
    glDeleteProgram(id);
}

VertexArrayObject::VertexArrayObject() {
    glGenVertexArrays(1, &id);
}

void VertexArrayObject::CreateVertexAttributePointer(int location, int length, int size, int type) {
    glVertexAttribPointer(location, size, type, GL_FALSE, length * size, nullptr);
    glEnableVertexAttribArray(location);
}

void VertexArrayObject::Bind() {
    glBindVertexArray(id);
}

void VertexArrayObject::Unbind() {
    glBindVertexArray(0);
}

template<typename T> BufferObject<T>::BufferObject() {
    glGenBuffers(1, &id);
}

template<typename T> BufferObject<T>::~BufferObject() {
    glDeleteBuffers(1, &id);
}

template<typename T> void BufferObject<T>::Upload(std::vector<T> data) {
    Bind();

    if (std::is_same<T, float>::value) {
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), GL_STATIC_DRAW);
    }
}

template<typename T> void BufferObject<T>::Bind() {
    if (std::is_same<T, float>::value) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }
    else if (std::is_same<T, int>::value) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }
}

template<typename T> void BufferObject<T>::Unbind() {
    if (std::is_same<T, float>::value) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (std::is_same<T, int>::value) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}