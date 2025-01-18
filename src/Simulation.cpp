#include <pch.h>
#include "Entity/CubeEntity.h"
#include "Simulation.h"

Simulation::Simulation()
{
    m_simulationState.RootWorldEntity.MakeChild<CubeEntity>();
}

SimulationState const& Simulation::Update(InputState const& inputState)
{
    if (m_lastUpdate == std::chrono::high_resolution_clock::time_point::min())
    {
        m_lastUpdate = std::chrono::high_resolution_clock::now();
        return m_simulationState;
    }
    auto currentTime{ std::chrono::high_resolution_clock::now() };
    auto deltaTime{ std::chrono::duration_cast<std::chrono::microseconds>(
        currentTime - m_lastUpdate) };
    m_simulationState.RootWorldEntity.Update(deltaTime, inputState);
    m_simulationState.Camera.Update(deltaTime, inputState);
    m_lastUpdate = currentTime;
    return m_simulationState;
}