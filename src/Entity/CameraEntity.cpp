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
{
    m_position = { 0.0f, 0.0f, -5.0f };
}

void CameraEntity::Update(std::chrono::microseconds deltaTime, InputState const &input)
{
    Eigen::Matrix4f rotationMatrix{ game::Rotation(m_rotation) };
    Eigen::Vector4f cameraDirection{ rotationMatrix * c_cameraTarget };
    if (input.MoveForward)
    {
        Eigen::Vector4f forwardVelocity{ cameraDirection * (c_cameraMovementSpeed * deltaTime.count()) };
        m_position += forwardVelocity.head<3>();
    }
    if (input.MoveBackward)
    {
        Eigen::Vector4f backwardVelocity{ cameraDirection * (c_cameraMovementSpeed * deltaTime.count()) };
        m_position -= backwardVelocity.head<3>();
    }
    m_rotation.y() += (input.RelativeLookX * c_cameraTurnSpeed);
    while (m_rotation.y() < static_cast<float>(-2*M_PI))
    {
        m_rotation.y() += static_cast<float>(2*M_PI);
    }
    while (m_rotation.y() > static_cast<float>(2*M_PI))
    {
        m_rotation.y() -= static_cast<float>(2*M_PI);
    }
    m_rotation.x() -= (input.RelativeLookY * c_cameraTurnSpeed);
    m_rotation.x() = std::clamp(m_rotation.x(), c_cameraMinPitch, c_cameraMaxPitch);
}