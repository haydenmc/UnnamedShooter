#pragma once
#include "Entities/Entity.h"
#include "Entities/CameraEntity.h"
#include "Input.h"

struct SimulationState
{
    Entity RootWorldEntity;
    CameraEntity Camera;
};

struct Simulation
{
    SimulationState const& Update(InputState const& inputState);
private:
    std::chrono::high_resolution_clock::time_point m_lastUpdate{
        std::chrono::high_resolution_clock::time_point::min() };
    SimulationState m_simulationState;
};