#pragma once
#include "../Input.h"
#include "../Simulation.h"

namespace game
{
struct RenderTarget;
struct Overlay
{
    virtual void Paint(RenderTarget* target, SimulationState const& simulationState) = 0;
};
}