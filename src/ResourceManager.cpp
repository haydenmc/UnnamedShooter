#include <pch.h>
#include "ResourceManager.h"

namespace game
{
std::unordered_map<TextPainterResourceKind, std::shared_ptr<TextPainter>>
    ResourceManager::m_textPainters;
std::unordered_map<MeshResourceKind, std::shared_ptr<Mesh>> ResourceManager::m_meshes;
void ResourceManager::Initialize()
{
    SPDLOG_INFO("ResourceManager initializing...");

    // Meshes
    m_meshes.insert_or_assign(MeshResourceKind::SandyLandscape,
        Mesh::FromObjFile("bigsandylandscape.obj", "bigsandylandscape.png"));

    // Text Painters
    m_textPainters.insert_or_assign(TextPainterResourceKind::Upheaval,
        TextPainter::FromBitmapFont("upheaval.fnt"));
}

std::shared_ptr<TextPainter> ResourceManager::GetTextPainter(TextPainterResourceKind kind)
{
    if (!m_textPainters.contains(kind))
    {
        LOG_AND_THROW("No text painter resource exists for kind '{}'", static_cast<int>(kind));
    }
    return m_textPainters.at(kind);
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(MeshResourceKind kind)
{
    if (!m_meshes.contains(kind))
    {
        LOG_AND_THROW("No mesh resource exists for kind '{}'", static_cast<int>(kind));
    }
    return m_meshes.at(kind);
}
}