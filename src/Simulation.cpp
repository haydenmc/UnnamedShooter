#include <pch.h>
#include "Entity/CubeEntity.h"
#include "Entity/LandscapeEntity.h"
#include "Entity/PlayerEntity.h"
#include "Painters/TextPainter.h"
#include "Simulation.h"

Simulation::Simulation()
{
    //m_simulationState.RootWorldEntity.MakeChild<CubeEntity>();
    auto playerEntity{ m_simulationState.RootWorldEntity
        .MakeChild<PlayerEntity>(&m_simulationState.Camera) };
    playerEntity->SetPosition(Eigen::Vector3f{ 0.0f, 0.0f, -5.0f });

    m_simulationState.RootWorldEntity.MakeChild<LandscapeEntity>();

    // REMOVEME
    auto upheavalTextPainter{ TextPainter::FromBitmapFont("upheaval.fnt") };
    // /REMOVEME
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