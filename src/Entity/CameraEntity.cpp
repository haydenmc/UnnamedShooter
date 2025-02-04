#include <pch.h>
#include "Entity.h"
#include "CameraEntity.h"

namespace
{
    constexpr float c_cameraMovementSpeed{ 0.000001f };
    constexpr float c_cameraTurnSpeed{ 0.0000005f };
    Eigen::Vector4f const c_cameraTarget{ 0.0f, 0.0f, 1.0f, 1.0f };
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
    if (input.MoveLeft)
    {
        m_rotation.y() -= (c_cameraTurnSpeed * deltaTime.count());
    }
    if (input.MoveRight)
    {
        m_rotation.y() += (c_cameraTurnSpeed * deltaTime.count());
    }
    SPDLOG_DEBUG("Camera position: {}", m_position);
}