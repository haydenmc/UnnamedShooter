#pragma once
#include "Mesh/Mesh.h"
#include "Painter/TextPainter.h"

namespace game
{
enum class TextPainterResourceKind
{
    UNKNOWN = 0,
    Upheaval,
    MAX_VALUE,
};

enum class MeshResourceKind
{
    UNKNOWN = 0,
    SandyLandscape,
    MAX_VALUE,
};

struct ResourceManager
{
    static void Initialize();
    static std::shared_ptr<TextPainter> GetTextPainter(TextPainterResourceKind kind);
    static std::shared_ptr<Mesh> GetMesh(MeshResourceKind kind);

private:
    static std::unordered_map<TextPainterResourceKind, std::shared_ptr<TextPainter>> m_textPainters;
    static std::unordered_map<MeshResourceKind, std::shared_ptr<Mesh>> m_meshes;
};
}