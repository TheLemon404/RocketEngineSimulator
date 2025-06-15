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
#include <cmath>

template struct BufferObject<float>;
template struct BufferObject<unsigned int>;

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

void VertexArrayObject::CleanUp() {
    glDeleteVertexArrays(1, &id);
}

template<typename T> BufferObject<T>::BufferObject() {
    glGenBuffers(1, &id);
}

template<typename T> BufferObject<T>::~BufferObject() {
    glDeleteBuffers(1, &id);
}

template<typename T> void BufferObject<T>::CleanUp() {
    glDeleteBuffers(1, &id);
}

template<typename T> void BufferObject<T>::Upload(std::vector<T> data) {
    Bind();
    if (std::is_same<T, float>::value) {
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    }
    else if (std::is_same<T, unsigned int>::value) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), data.data(), GL_DYNAMIC_DRAW);
    }
}

template<typename T> void BufferObject<T>::Bind() {
    if (std::is_same<T, float>::value) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }
    else if (std::is_same<T, unsigned int>::value) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }
}

template<typename T> void BufferObject<T>::Unbind() {
    if (std::is_same<T, float>::value) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (std::is_same<T, unsigned int>::value) {
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

Model Model::LoadModelFromOBJ(std::string localPath) {
    Assimp::Importer Importer;

    const aiScene* scene = Importer.ReadFile(localPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    std::vector<Mesh> resultMeshes;
    resultMeshes.resize(scene->mNumMeshes);

    if (scene) {
        for (int i = 0; i < scene->mNumMeshes; i++) {
            if (scene->mMeshes[i]) {
                for (unsigned int j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {
                    const aiVector3D* pPos = &(scene->mMeshes[i]->mVertices[j]);
                    const aiVector3D* pNormal = &(scene->mMeshes[i]->mNormals[j]);
                    if (!scene->mMeshes[i]->HasTextureCoords(0)) {
                        throw std::runtime_error("imported OBJ mesh must have uvs");
                    }
                    const aiVector3D* pTexCoord = &(scene->mMeshes[i]->mTextureCoords[0][j]);

                    resultMeshes[i].vertices.push_back(pPos->x);
                    resultMeshes[i].vertices.push_back(pPos->y);
                    resultMeshes[i].vertices.push_back(pPos->z);
                    resultMeshes[i].normals.push_back(pNormal->x);
                    resultMeshes[i].normals.push_back(pNormal->y);
                    resultMeshes[i].normals.push_back(pNormal->z);
                    resultMeshes[i].uvs.push_back(pTexCoord->x);
                    resultMeshes[i].uvs.push_back(pTexCoord->y);
                }

                int materialIndex = scene->mMeshes[i]->mMaterialIndex;
                aiMaterial* material = scene->mMaterials[materialIndex];
                aiColor3D color;
                if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                    resultMeshes[i].material.color.r = color.r;
                    resultMeshes[i].material.color.g = color.g;
                    resultMeshes[i].material.color.b = color.b;
                }

                for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
                    const aiFace& Face = scene->mMeshes[i]->mFaces[j];
                    assert(Face.mNumIndices == 3);
                    resultMeshes[i].indices.push_back(Face.mIndices[0]);
                    resultMeshes[i].indices.push_back(Face.mIndices[1]);
                    resultMeshes[i].indices.push_back(Face.mIndices[2]);
                }
            }
        }
    }
    Model model = {resultMeshes};
    return model;
}

void Mesh::UpdateBuffers() {
    vao->Bind();

    positionsBuffer->Upload(vertices);
    indicesBuffer->Upload(indices);

    vao->Unbind();
}

void Scene::CleanUp() {
    for (int i = 0; i < models.size(); i++) {
        for (int j = 0; j < models[i].meshes.size(); j++) {
            models[i].meshes[j].vao->CleanUp();
            models[i].meshes[j].positionsBuffer->CleanUp();
            models[i].meshes[j].uvsBuffer->CleanUp();
            models[i].meshes[j].normalsBuffer->CleanUp();
            models[i].meshes[j].indicesBuffer->CleanUp();
        }
    }
}

std::vector<glm::vec3> Control::RecursiveBevel(glm::vec3 a, glm::vec3 v, glm::vec3 b, float distance, int currentDepth) {
    if (currentDepth == bevelNumber) {
        glm::vec3 dir1 = normalize(a - v);
        glm::vec3 dir2 = normalize(b - v);

        // Calculate the angle between directions
        float cosAngle = glm::dot(dir1, dir2);
        float angle = acos(glm::clamp(cosAngle, -1.0f, 1.0f));

        // Adjust distance based on angle to prevent overlapping
        // For sharp angles, we need to reduce the distance more
        float angleAdjustment = sin(angle * 0.5f); // This gives us a factor between 0 and 1
        float adjustedDistance = distance * angleAdjustment;

        // Ensure minimum distance to prevent complete collapse
        adjustedDistance = glm::max(adjustedDistance, distance * 0.1f);

        glm::vec3 v1 = v + dir1 * adjustedDistance;
        glm::vec3 v2 = v + dir2 * adjustedDistance;

        // Additional check: if points are too close, separate them
        float minSeparation = distance * 0.2f;
        if (glm::distance(v1, v2) < minSeparation) {
            glm::vec3 separation = glm::normalize(v2 - v1) * (minSeparation * 0.5f);
            v1 -= separation;
            v2 += separation;
        }

        return {v1, v2};
    }

    glm::vec3 dir1 = normalize(v - a);
    glm::vec3 dir2 = normalize(b - v);

    // Calculate angle adjustment for intermediate points too
    float cosAngle = glm::dot(-dir1, dir2);
    float angle = acos(glm::clamp(cosAngle, -1.0f, 1.0f));
    float angleAdjustment = sin(angle * 0.5f);
    float adjustedDistance = distance * angleAdjustment;
    adjustedDistance = glm::max(adjustedDistance, distance * 0.1f);

    glm::vec3 v1 = v - dir1 * adjustedDistance;
    glm::vec3 v2 = v + dir2 * adjustedDistance;

    float newDistance = adjustedDistance / 2.0f;

    std::vector<glm::vec3> left = RecursiveBevel(a, v1, v2, newDistance, currentDepth + 1);
    std::vector<glm::vec3> right = RecursiveBevel(v1, v2, b, newDistance, currentDepth + 1);
    left.insert(left.end(), right.begin(), right.end());
    return left;
}

int Control::GetNumBeveledVertices() {
    return pow(2, bevelNumber);
}

void Control::CheckSelection(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution) {
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

std::vector<glm::vec3> Control::ExtractBeveledPositions(glm::vec3 prevPoint, glm::vec3 nextPoint) {
    return RecursiveBevel(prevPoint, position, nextPoint, bevelRadius, 1);
}

glm::vec3 LinePath::RoundToMajorAxis(const glm::vec3 &v) {
    glm::vec3 abs_v = glm::abs(v);

    if (abs_v.x >= abs_v.y && abs_v.x >= abs_v.z) {
        return {v.x >= 0 ? 1.0f : -1.0f, 0.0f, 0.0f};
    } else if (abs_v.y >= abs_v.z) {
        return {0.0f, v.y >= 0 ? 1.0f : -1.0f, 0.0f};
    } else {
        return {0.0f, 0.0f, v.z >= 0 ? 1.0f : -1.0f};
    }
}

glm::vec3 LinePath::RountToMajorPlane(const glm::vec3 &v) {
    glm::vec3 majorAxis = RoundToMajorAxis(v);
    glm::vec3 secondaryAxis = RoundToMajorAxis(v - (v * abs(majorAxis)));
    return majorAxis + secondaryAxis;
}


void LinePath::UpdatePositionsArray() {
    positions.clear();
    for (int i = 0; i < controls.size(); i++) {
        if (controls[i].bevelNumber == 0) {
            positions.push_back(controls[i].position.x);
            positions.push_back(controls[i].position.y);
            positions.push_back(controls[i].position.z);
        }
        else {
            std::vector<glm::vec3> beveledPositions = controls[i].ExtractBeveledPositions(controls[i - 1].position, controls[i + 1].position);
            for (glm::vec3& position : beveledPositions) {
                positions.push_back(position.x);
                positions.push_back(position.y);
                positions.push_back(position.z);
            }
        }
    }
}

std::vector<glm::vec3> LinePath::ExtractPositions() {
    std::vector<glm::vec3> result;
    for (int i = 0; i < controls.size(); i++) {
        if (controls[i].bevelNumber == 0) {
            result.push_back(controls[i].position);
        }
        else {
            std::vector<glm::vec3> beveledPositions = controls[i].ExtractBeveledPositions(controls[i - 1].position, controls[i + 1].position);
            for (glm::vec3& position : beveledPositions) {
                result.push_back(position);
            }
        }
    }
    return result;
}

int LinePath::GetSelectedControlIndex(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution) {
    for (int i = 0; i < controls.size(); i++) {
        controls[i].CheckSelection(mousePosition, view, projection, screenResolution);
    }
    for (int i = 0; i < controls.size(); i++) {
        if (controls[i].selected) {
            return i;
        }
    }

    return -1;
}

int LinePath::Extrude(int controlIndex, glm::vec3 to) {
    glm::vec3 origin = controls[controlIndex].position;
    glm::vec3 vectorLength = to - origin;
    glm::vec3 axis = RoundToMajorAxis(abs(normalize(to - origin)));

    if (controlIndex > 0) {
        Control s = {origin + (vectorLength * axis)};
        controls[controls.size() - 1].bevelNumber = 3;
        controls.push_back(s);
        return controls.size() - 1;
    }

    Control s = {origin + (vectorLength * axis)};
    controls[0].bevelNumber = 3;
    controls.insert(controls.begin(), s);
    return 0;
}

void LinePath::UpdatePositionsBuffer() {
    UpdatePositionsArray();
    positionsBuffer->Upload(positions);
}

int LinePath::GetNumVertices() {
    int sum = 0;
    for (int i = 0; i < controls.size(); i++) {
        sum += pow(2, controls[i].bevelNumber);
    }
    return sum;
}

void Pipe::UpdatePositionsBuffer() {
    UpdateArrays();
    positionsBuffer->Upload(positions);
    normalsBuffer->Upload(normals);
    indicesBuffer->Upload(indices);
}

// Updated methods for Pipe class to fix twisting at beveled corners
std::vector<glm::vec3> Pipe::GenerateRingWithFrame(glm::vec3 center, glm::vec3 tangent,
                                                   glm::vec3 right, glm::vec3 up, float radius) {
    std::vector<glm::vec3> ring;

    // Generate vertices around the ring using the provided frame
    for (int i = 0; i < segments; i++) {
        float angle = (2.0f * M_PI * i) / segments;
        glm::vec3 point = center + radius * (cos(angle) * right + sin(angle) * up);
        ring.push_back(point);
    }

    return ring;
}

// Helper function to transport a frame along the path
void Pipe::TransportFrame(glm::vec3 prevTangent, glm::vec3 newTangent,
                         glm::vec3& right, glm::vec3& up) {
    // If tangents are nearly parallel, no rotation needed
    if (glm::length(glm::cross(prevTangent, newTangent)) < 1e-6f) {
        return;
    }

    // Calculate rotation axis and angle
    glm::vec3 rotationAxis = glm::normalize(glm::cross(prevTangent, newTangent));
    float rotationAngle = acos(glm::clamp(glm::dot(prevTangent, newTangent), -1.0f, 1.0f));

    // Create rotation matrix
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

    // Apply rotation to the frame vectors
    right = glm::vec3(rotationMatrix * glm::vec4(right, 0.0f));
    up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));
}

void Pipe::UpdateArrays() {
    positions.clear();
    normals.clear();
    indices.clear();

    std::vector<glm::vec3> points = path.ExtractPositions();

    if (points.size() < 2) {
        return;
    }

    int controlIndex = 0;
    int numVertsForCurrentControl = 0;

    glm::vec3 tangent = glm::normalize(points[1] - points[0]);

    // Create initial perpendicular vectors (avoiding gimbal lock)
    glm::vec3 right, up;
    if (abs(tangent.y) < 0.9f) {
        right = glm::normalize(glm::cross(tangent, glm::vec3(0, 1, 0)));
    } else {
        right = glm::normalize(glm::cross(tangent, glm::vec3(1, 0, 0)));
    }
    up = glm::normalize(glm::cross(right, tangent));

    // Generate vertices for each point along the path
    for (int i = 0; i < points.size(); i++) {
        glm::vec3 newTangent;

        // Calculate tangent direction
        if (i == 0) {
            newTangent = glm::normalize(points[i + 1] - points[i]);
        } else if (i == points.size() - 1) {
            newTangent = glm::normalize(points[i] - points[i - 1]);
        } else {
            // For middle points, use the direction to the next point
            // This avoids the averaging that causes twisting
            newTangent = glm::normalize(points[i + 1] - points[i]);
        }

        // Transport the frame to maintain continuity
        if (i > 0) {
            TransportFrame(tangent, newTangent, right, up);
        }
        tangent = newTangent;

        std::vector<glm::vec3> ring = GenerateRingWithFrame(points[i], tangent, right, up, path.controls[controlIndex].radius);
        for (const glm::vec3& p : ring) {
            glm::vec3 normal = normalize(p - points[i]);
            positions.push_back(p.x);
            positions.push_back(p.y);
            positions.push_back(p.z);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }

        numVertsForCurrentControl++;
        if (numVertsForCurrentControl >= path.controls[controlIndex].GetNumBeveledVertices()) {
            numVertsForCurrentControl = 0;
            controlIndex++;
        }
    }

    // Generate indices to connect the rings
    for (int i = 0; i < points.size() - 1; i++) {
        for (int v = 0; v < segments; v++) {
            // Current ring vertices
            int current = i * segments + v;
            int currentNext = i * segments + ((v + 1) % segments);

            // Next ring vertices
            int next = (i + 1) * segments + v;
            int nextNext = (i + 1) * segments + ((v + 1) % segments);

            // Create two triangles to form a quad between rings
            // Triangle 1: current -> next -> currentNext
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(currentNext);

            // Triangle 2: currentNext -> next -> nextNext
            indices.push_back(currentNext);
            indices.push_back(next);
            indices.push_back(nextNext);
        }
    }
}

