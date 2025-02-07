#include <pch.h>
#include "Entity.h"
#include "CameraEntity.h"

namespace
{
    constexpr float c_cameraMovementSpeed{ 0.000001f };
    constexpr float c_cameraTurnSpeed{ 0.005f };
    Eigen::Vector4f const c_cameraTarget{ 0.0f, 0.0f, 1.0f, 1.0f };
    constexpr float c_cameraMinPitch{(-static_cast<float>(M_PI_2) + 0.1f)};
    constexpr float c_cameraMaxPitch{(static_cast<float>(M_PI_2) - 0.1f)};
}

CameraEntity::CameraEntity()
{ }

void CameraEntity::Update(std::chrono::microseconds /*deltaTime*/, InputState const &/*input*/)
{ }