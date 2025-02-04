#pragma once
#include "Entity.h"

struct CameraEntity : Entity
{
    CameraEntity();
    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input) override;
};