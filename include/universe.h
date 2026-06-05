#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <cmath>

constexpr float metersPerUnit = 1.0f;
constexpr float defaultSimulationScale = 1.0f / metersPerUnit;
constexpr float defaultGravConstant = 6.6743e-11 * (1.0f / (defaultSimulationScale));

constexpr float timeStep = 0.001f; 

inline float gravitationalConstant = 10.0f;  
inline float simulationScale = 1.0f;    
inline float timeScale = 1.0f;              

inline void setGravConstant(float newVal) {
    gravitationalConstant = newVal;
}

inline void resetGravConstant() {
    gravitationalConstant = defaultGravConstant; 
}

#endif