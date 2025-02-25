#pragma once
#include "Overlay.h"
#include "../Painter/TextPainter.h"

namespace game
{
struct DebugOverlay : public Overlay
{
    DebugOverlay();
    virtual void Paint(RenderTarget* target, SimulationState const& simulationState) override;

private:
    std::shared_ptr<TextPainter> const m_textPainter;
    std::chrono::high_resolution_clock::time_point m_lastPaint;
};
}