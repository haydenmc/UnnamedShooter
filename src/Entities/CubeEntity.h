#pragma once
#include "Entity.h"
#include "../Mesh.h"

struct CubeEntity : public Entity
{
    CubeEntity()
    {
        m_meshes.push_back(Mesh::Cube());
    }

    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input) override
    {
        Entity::Update(deltaTime, input);

        m_rotation.y += 0.000001f * deltaTime.count();
    }
};