#pragma once

struct MeshFace
{
    std::array<size_t, 3> MeshVertexIndices;
    // std::array<glm::vec2, 3> VertexTextureCoordinates;
};

struct Mesh
{
    std::vector<Vector3> Vertices;
    std::vector<MeshFace> Faces;
    // glm::vec3 Rotation;
    // glm::vec3 Scale;
    // glm::vec3 Translation;

    static std::shared_ptr<Mesh> FromObjFile(std::filesystem::path filePath);
    static std::shared_ptr<Mesh> Cube();
    static std::shared_ptr<Mesh> AdjoiningTriangles();
};