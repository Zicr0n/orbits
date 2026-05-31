#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <cmath>

constexpr float timeStep = 1.0f / 60.0f;
constexpr float metersPerUnit = 1.0f;
constexpr float defaultSimulationScale = 1.0f / metersPerUnit;
constexpr float defaultGravConstant = 6.674e-11f * (1.0f / defaultSimulationScale);

inline float simulationScale = defaultSimulationScale;
inline float gravitationalConstant = 1.0f;
inline float timeScale = 1.0f;

inline void setGravConstant(float newVal) {
    gravitationalConstant = newVal;
}

inline void resetGravConstant() {
    gravitationalConstant = defaultGravConstant; 
}

#endif