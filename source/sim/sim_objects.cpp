//
// Created by Osprey on 6/7/2025.
//

#include "sim_objects.h"

std::vector<float> Spline::ExtractPositions() {
    return std::vector<float>({p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z});
}
