//
// Created by Osprey on 6/4/2025.
//

#include "graphics_objects.h"

#include "../core/io.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

template struct BufferObject<float>;
template struct BufferObject<int>;

void Camera::RotateAround(float angle, glm::vec3 axis, glm::vec3 originPoint) {
    // Create the transformation matrix
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -originPoint);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    glm::mat4 inverseTranslationMatrix = glm::translate(glm::mat4(1.0f), originPoint);
    glm::mat4 finalTransformation = inverseTranslationMatrix * rotationMatrix * translationMatrix;

    // Apply the transformation to a point
    glm::vec4 originalPoint = glm::vec4(position, 1.0f); // Assuming the object's origin
    position = finalTransformation * originalPoint;
}

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

void ShaderProgramObject::UploadUniformFloat(const char *uniformName, float value) {
    glUniform1f(glGetUniformLocation(id, uniformName), value);
}

void ShaderProgramObject::UploadUniformInt(const char *uniformName, int value) {
    glUniform1i(glGetUniformLocation(id, uniformName), value);
}

void ShaderProgramObject::UploadUniformVec2(const char *uniformName, glm::vec2 value) {
    glUniform2fv(glGetUniformLocation(id, uniformName), 1, glm::value_ptr(value));
}

void ShaderProgramObject::UploadUniformVec3(const char *uniformName, glm::vec3 value) {
    glUniform3fv(glGetUniformLocation(id, uniformName), 1, glm::value_ptr(value));
}

void ShaderProgramObject::UploadUniformVec4(const char *uniformName, glm::vec4 value) {
    glUniform4fv(glGetUniformLocation(id, uniformName), 1, glm::value_ptr(value));
}

void ShaderProgramObject::UploadUniformMat4(const char *uniformName, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(id, uniformName), 1, GL_FALSE, glm::value_ptr(value));
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
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    }
    else if (std::is_same<T, int>::value) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), data.data(), GL_STATIC_DRAW);
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