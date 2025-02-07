#include "PlayerEntity.h"
#include "CameraEntity.h"

namespace
{
    constexpr float c_movementAcceleration{ 0.00000000005f };
    constexpr float c_maxMovementVelocity{  0.000005f };
    constexpr float c_movementFriction{     0.00000000004f };
    Eigen::Vector3f const c_forwardMovementDirection{   0.0f,  0.0f,  1.0f };
    Eigen::Vector3f const c_backwardMovementDirection{  0.0f,  0.0f, -1.0f };
    Eigen::Vector3f const c_leftMovementDirection{     -1.0f,  0.0f,  0.0f };
    Eigen::Vector3f const c_rightMovementDirection{     1.0f,  0.0f,  0.0f };
    constexpr float c_velocityMagnitudeDeadzone{ 0.0000001f };
    constexpr float c_lookSensitivity{ 0.005f };
}

PlayerEntity::PlayerEntity(CameraEntity *camera) : m_camera{ camera }
{ }

void PlayerEntity::Update(std::chrono::microseconds deltaTime, InputState const &input)
{
    // Apply acceleration based on yaw angle only
    Eigen::Vector3f movementRotation{ 0.0f, m_rotation.y(), 0.0f };
    Eigen::Matrix4f movementRotationMatrix{ game::Rotation(movementRotation) };

    // Determine acceleration direction based on input
    Eigen::Vector3f accelerationInput{ Eigen::Vector3f::Zero() };
    if (input.MoveForward)  { accelerationInput += c_forwardMovementDirection; }
    if (input.MoveBackward) { accelerationInput += c_backwardMovementDirection; }
    if (input.MoveLeft)     { accelerationInput += c_leftMovementDirection; }
    if (input.MoveRight)    { accelerationInput += c_rightMovementDirection; }
    accelerationInput.normalize();
    Eigen::Vector4f accelerationDirection{ accelerationInput.x(), accelerationInput.y(),
        accelerationInput.z(), 1.0f };
    accelerationDirection = movementRotationMatrix * accelerationDirection;

    // Apply acceleration in appropriate magnitude
    Eigen::Vector3f acceleration{ accelerationDirection.head<3>() *
        (c_movementAcceleration * deltaTime.count()) };
    m_velocity += acceleration;

    // Apply friction
    {
        Eigen::Vector3f frictionDirection{ -m_velocity };
        frictionDirection.normalize();
        Eigen::Vector3f frictionAcceleration{
            frictionDirection * (c_movementFriction * deltaTime.count()) };
        m_velocity += frictionAcceleration;
    }

    // Apply deadzone to avoid little tiny movement due to bad precision
    if (accelerationInput.isZero() && (m_velocity.norm() < c_velocityMagnitudeDeadzone))
    {
        m_velocity.setZero();
    }

    // Clamp to maximum velocity
    if (m_velocity.norm() > c_maxMovementVelocity)
    {
        m_velocity.normalize();
        m_velocity *= c_maxMovementVelocity;
    }

    // Apply player velocity to player position
    Eigen::Vector3f movementDelta{ m_velocity * deltaTime.count() };
    m_position += movementDelta;

    // Apply mouse look
    m_rotation.y() += (input.RelativeLookX * c_lookSensitivity);
    m_lookPitchRadians -= (input.RelativeLookY * c_lookSensitivity);

    // Snap camera to player's "eye"
    Eigen::Vector3f cameraRotation{ m_lookPitchRadians, m_rotation.y(), 0.0f };
    m_camera->SetPosition(m_position);
    m_camera->SetRotation(cameraRotation);

    SPDLOG_DEBUG("Player velocity: {}", m_velocity.norm());
}