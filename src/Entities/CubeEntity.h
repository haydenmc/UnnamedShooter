#pragma once
#include "Entity.h"
#include "../Mesh.h"

struct CubeEntity : public Entity
{
    CubeEntity()
    {
        m_meshes.push_back(Mesh::Cube());

        m_position.z = FixedUnit{ 10 };
    }

    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input) override
    {
        Entity::Update(deltaTime, input);

        m_position.z -= FixedUnit{0.000001f * deltaTime.count()};

        //m_rotation.y += ;
    }
};