#include <pch.h>
#include "DebugOverlay.h"
#include "../ResourceManager.h"

namespace game
{
DebugOverlay::DebugOverlay() : m_textPainter{
    ResourceManager::GetTextPainter(TextPainterResourceKind::Upheaval) }
{ }

void DebugOverlay::Paint(RenderTarget* target, SimulationState const& /*simulationState*/)
{
    auto now{ std::chrono::high_resolution_clock::now() };
    auto paintDeltaTime{ std::chrono::duration_cast<std::chrono::microseconds>(
        now - m_lastPaint) };
    auto fps{ 1'000'000 / paintDeltaTime.count() };
    m_textPainter->PaintText(target, 0, 0, fmt::format("FPS: {}", fps));
    m_lastPaint = now;
}
}