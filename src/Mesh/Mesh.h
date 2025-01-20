#pragma once
#include "../Texture/PngTexture.h"

struct MeshFace
{
    std::array<size_t, 3> MeshVertexIndices;
    std::array<size_t, 3> MeshTextureCoordinateIndices;
    uint32_t ShadeColor;
};

struct Mesh
{
    std::vector<Vector3> Vertices;
    std::vector<Vector2> TextureCoordinates;
    std::vector<MeshFace> Faces;
    // glm::vec3 Rotation;
    // glm::vec3 Scale;
    // glm::vec3 Translation;
    std::shared_ptr<PngTexture> Texture;

    static std::shared_ptr<Mesh> FromObjFile(std::filesystem::path objFilePath,
        std::optional<std::filesystem::path> textureFilePath);
    static std::shared_ptr<Mesh> Cube();
    static std::shared_ptr<Mesh> AdjoiningTriangles();
};