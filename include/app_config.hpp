#pragma once

#include <array>

namespace app {

inline constexpr const char* kWindowTitle = "Tube Renderer";
inline constexpr unsigned int kWindowWidth = 800;
inline constexpr unsigned int kWindowHeight = 600;

inline constexpr float kTubeInnerRadius = 0.6f;
inline constexpr float kTubeOuterRadius = 1.0f;
inline constexpr float kTubeHeight = 2.0f;
inline constexpr int kTubeSegments = 32;

inline constexpr float kLightOrbitRadius = 5.0f;
inline constexpr float kLightSphereRadius = 0.5f;
inline constexpr int kLightSphereSectors = 32;
inline constexpr int kLightSphereStacks = 16;

inline constexpr unsigned int kVertexStrideFloats = 8;

inline constexpr std::array<float, 32> kGroundVertices = {
    -100.0f, -1.0f, -100.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
     100.0f, -1.0f, -100.0f, 0.0f, 1.0f, 0.0f, 50.0f, 0.0f,
     100.0f, -1.0f,  100.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f,
    -100.0f, -1.0f,  100.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f,
};

inline constexpr std::array<unsigned int, 6> kGroundIndices = {
    0, 1, 2,
    2, 3, 0,
};

}  // namespace app
