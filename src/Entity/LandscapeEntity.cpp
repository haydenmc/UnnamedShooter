#include "LandscapeEntity.h"
#include "../Mesh/Mesh.h"
#include "../ResourceManager.h"

LandscapeEntity::LandscapeEntity()
{
    m_position.y() = 2.0f;
    //m_rotation.x() = static_cast<float>(M_PI);
    m_meshes.push_back(game::ResourceManager::GetMesh(game::MeshResourceKind::SandyLandscape));
}
