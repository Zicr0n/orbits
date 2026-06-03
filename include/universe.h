#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <cmath>

constexpr float metersPerUnit = 100.0f;
constexpr float defaultSimulationScale = 1.0f / metersPerUnit;
constexpr float defaultGravConstant = 6.6743e-11 * (1.0f / (defaultSimulationScale));

// universe.h
constexpr float timeStep = 1.0f / 60.0f; 

inline float gravitationalConstant = 1.0f; 
inline float simulationScale = defaultSimulationScale;    
inline float timeScale = 1.0f;              

inline void setGravConstant(float newVal) {
    gravitationalConstant = newVal;
}

inline void resetGravConstant() {
    gravitationalConstant = defaultGravConstant; 
}

#endif