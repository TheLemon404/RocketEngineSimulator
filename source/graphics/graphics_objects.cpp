//
// Created by Osprey on 6/4/2025.
//

#include "graphics_objects.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stbi/stb_image.h"
#include "../core/io.h"
#include "assimp/Exceptional.h"
#include "assimp/Importer.hpp"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
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
    if (id != 0) glDeleteShader(id);
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

void ShaderProgramObject::CompileTesselation(ShaderObject *vertexShader, ShaderObject *controlShader, ShaderObject *evaluationShader, ShaderObject *fragmentShader) {
    vertexShaderObject = vertexShader;
    fragmentShaderObject = fragmentShader;
    tesselationControlShaderObject = controlShader;
    tesselationEvaluationShaderObject = evaluationShader;

    glAttachShader(id, vertexShader->id);
    glAttachShader(id, fragmentShader->id);
    glAttachShader(id, controlShader->id);
    glAttachShader(id, evaluationShader->id);
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
    delete tesselationControlShaderObject;
    delete tesselationEvaluationShaderObject;

    glDeleteProgram(id);
}

VertexArrayObject::VertexArrayObject() {
    glGenVertexArrays(1, &id);
}

void VertexArrayObject::CreateVertexAttributePointer(int location, int length, int size, int type) {
    glVertexAttribPointer(location, length, type, GL_FALSE, length * size, (void*)0);
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

TextureObject::TextureObject(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    glGenTextures(1, &id);
}

void TextureObject::Load(std::string localPath) {
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(localPath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void TextureObject::SetData(void *data) {
    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void TextureObject::Bind() {
    glBindTexture(GL_TEXTURE_2D, id);
}

void TextureObject::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh Mesh::LoadmeshFromOBJ(std::string localPath, int meshIndex) {
    Assimp::Importer Importer;

    Mesh resultMesh{};

    const aiScene* scene = Importer.ReadFile(localPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    aiMesh* mesh;

    if (scene) {
        mesh = scene->mMeshes[meshIndex];
        if (mesh) {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                const aiVector3D* pPos = &(mesh->mVertices[i]);
                const aiVector3D* pNormal = &(mesh->mNormals[i]);
                if (!mesh->HasTextureCoords(0)) {
                    throw std::runtime_error("imported OBJ mesh must have uvs");
                }
                const aiVector3D* pTexCoord = &(mesh->mTextureCoords[0][i]);

                resultMesh.vertices.push_back(pPos->x);
                resultMesh.vertices.push_back(pPos->y);
                resultMesh.vertices.push_back(pPos->z);
                resultMesh.normals.push_back(pNormal->x);
                resultMesh.normals.push_back(pNormal->y);
                resultMesh.normals.push_back(pNormal->z);
                resultMesh.uvs.push_back(pTexCoord->x);
                resultMesh.uvs.push_back(pTexCoord->y);
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                const aiFace& Face = mesh->mFaces[i];
                assert(Face.mNumIndices == 3);
                resultMesh.indices.push_back(Face.mIndices[0]);
                resultMesh.indices.push_back(Face.mIndices[1]);
                resultMesh.indices.push_back(Face.mIndices[2]);
            }

            return resultMesh;
        }

        throw runtime_error("Failed to load mesh file: " + localPath);
    }

    throw runtime_error("Failed to load scene for mesh file: " + localPath);
}

void Mesh::UpdateBuffers() {
    vao->Bind();

    positionsBuffer->Upload(vertices);
    indicesBuffer->Upload(indices);

    vao->Unbind();
}

void SelectableSpace::CheckSelection(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution) {
    //convert world space to screen space
    glm::vec4 worldPosition = glm::vec4(position.x, position.y, position.z, 1.0f);
    glm::vec4 viewPosition = view * worldPosition;
    glm::vec4 clipPosition = projection * viewPosition;
    glm::vec3 ndcPos = glm::vec3(clipPosition.x / clipPosition.w, clipPosition.y / clipPosition.w, clipPosition.z / clipPosition.w);

    glm::vec2 screenCoords = glm::ivec2((ndcPos.x + 1.0f) * 0.5f * screenResolution.x, (1.0f - ndcPos.y) * 0.5f * screenResolution.y);

    if (glm::distance(screenCoords, mousePosition) < radius * 100.0f) {
        selected = true;
    }
    else {
        selected = false;
    }
}

std::vector<float> Spline::ExtractPositions() {
    return std::vector<float>{
        p0.position.x, p0.position.y, p0.position.z, p1.position.x, p1.position.y, p1.position.z, p2.position.x,
        p2.position.y, p2.position.z, p3.position.x, p3.position.y, p3.position.z
    };
}

SelectableSpace* Spline::GetSelectedGizmo(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution) {
    p0.CheckSelection(mousePosition, view, projection, screenResolution);
    p1.CheckSelection(mousePosition, view, projection, screenResolution);
    p2.CheckSelection(mousePosition, view, projection, screenResolution);
    p3.CheckSelection(mousePosition, view, projection, screenResolution);

    if (p0.selected) {
        return &p0;
    }
    else if (p1.selected) {
        return &p1;
    }
    else if (p2.selected) {
        return &p2;
    }
    else if (p3.selected) {
        return &p3;
    }

    return nullptr;
}


