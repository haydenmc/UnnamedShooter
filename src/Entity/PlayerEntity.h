#pragma once
#include "Entity.h"

struct CameraEntity;

struct PlayerEntity : Entity
{
    PlayerEntity(CameraEntity* camera);
    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input) override;
private:
    CameraEntity *const m_camera;
    Eigen::Vector3f m_velocity{ 0.0f, 0.0f, 0.0f };
    float m_lookPitchRadians{ 0.0f };
};