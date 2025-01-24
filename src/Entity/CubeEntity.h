#pragma once
#include "Entity.h"
#include "../Mesh/Mesh.h"

struct CubeEntity : public Entity
{
    CubeEntity()
    {
        //m_meshes.push_back(Mesh::FromObjFile("cube.obj", "cube.png"));
        //m_meshes.push_back(Mesh::FromObjFile("f22.obj", "f22.png"));
        //m_meshes.push_back(Mesh::Cube());
        m_meshes.push_back(Mesh::AdjoiningTriangles());
        m_position.z = FixedUnit{ -5 };
    }

    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input) override
    {
        Entity::Update(deltaTime, input);
        //m_rotation.y += FixedUnit{ 0.000001f * deltaTime.count() };
        //m_rotation.x += FixedUnit{ 0.000001f * deltaTime.count() };
        m_position.x -= FixedUnit{ 0.000001f * deltaTime.count() };
    }
};