#pragma once

#include <vector>

struct MeshData {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

MeshData generateSphere(float radius, int sectorCount, int stackCount);
MeshData generateTube(float innerRadius, float outerRadius, float height, int segments);
