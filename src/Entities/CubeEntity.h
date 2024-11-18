#pragma once
#include "Entity.h"
#include "../Mesh.h"

struct CubeEntity : public Entity
{
    CubeEntity()
    {
        m_meshes.push_back(Mesh::Cube());
    }
};