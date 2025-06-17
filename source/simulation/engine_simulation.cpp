//
// Created by Osprey on 6/17/2025.
//

#include "engine_simulation.h"

int ConnectableModel::GetCurrentConnectionPointIndex(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution) {
    for (int i = 0; i < connectionPoints.size(); i++) {
        //convert world space to screen space
        glm::vec4 worldPosition = glm::vec4(position.x, position.y, position.z, 1.0f);
        glm::vec4 viewPosition = view * worldPosition;
        glm::vec4 clipPosition = projection * viewPosition;
        glm::vec3 ndcPos = glm::vec3(clipPosition.x / clipPosition.w, clipPosition.y / clipPosition.w, clipPosition.z / clipPosition.w);
        glm::vec2 screenCoords = glm::ivec2((ndcPos.x + 1.0f) * 0.5f * screenResolution.x, (1.0f - ndcPos.y) * 0.5f * screenResolution.y);

        if (glm::distance(screenCoords, mousePosition) < radius * 100.0f) {
            return i;
        }
    }

    return -1;
}

Tank::Tank(Gas storedGas, float volume, float storedAmount) : ConnectableModel(Model::LoadModelFromOBJ("resources/meshes/tank.obj")), storedGas(storedGas), volume(volume), storedAmount(storedAmount) {
    meshes[0].material.color = storedGas.color;
}

ElectricPump::ElectricPump() : Pump(Model::LoadModelFromOBJ("resources/meshes/pump.obj")) {
    meshes[1].material.color = glm::vec3(0.8, 0.6, 0.4);

    connectionPoints.push_back(glm::vec3(0));
}
