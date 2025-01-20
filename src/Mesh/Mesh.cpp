#include <pch.h>
#include "Mesh.h"

namespace
{
    constexpr size_t c_verticesPerFace{ 3 };
    constexpr std::array<uint32_t, 6> c_faceColors{
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFF00,
        0xFFFF00FF,
        0xFF00FFFF,
    };
}

std::shared_ptr<Mesh> Mesh::FromObjFile(std::filesystem::path objFilePath,
    std::optional<std::filesystem::path> textureFilePath)
{
    SPDLOG_INFO("Loading mesh from OBJ file '{}'...", objFilePath.string());
    if (!std::filesystem::exists(objFilePath))
    {
        SPDLOG_ERROR("OBJ file '{}' does not exist", objFilePath.string());
        return nullptr;
    }
    std::ifstream objFile{ objFilePath, std::ios::in };
    if (!objFile.is_open())
    {
        SPDLOG_ERROR("Could not open OBJ file '{}' for reading", objFilePath.string());
        return nullptr;
    }

    std::vector<Vector3> vertices;
    std::vector<Vector2> textureCoordinates;
    std::vector<MeshFace> faces;
    std::string line;
    while (std::getline(objFile, line))
    {
        std::istringstream iss{ line };
        std::string prefix;
        iss >> prefix;
        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(FixedUnit{ x }, FixedUnit{ y }, FixedUnit{ z });
        }
        else if (prefix == "vt")
        {
            float u, v;
            iss >> u >> v;
            textureCoordinates.emplace_back(FixedUnit{ u }, FixedUnit{ v });
        }
        else if (prefix == "vn")
        {
            // Skip for now
        }
        else if (prefix == "f")
        {
            std::string faceStr;
            std::vector<size_t> vertexIndices;
            std::vector<size_t> textureCoordinateIndices;
            while (iss >> faceStr)
            {
                auto slashCount{ std::count(faceStr.begin(), faceStr.end(), '/') };
                if (slashCount == 0)
                {
                    vertexIndices.push_back(std::stoi(faceStr));
                }
                else if (slashCount == 1)
                {
                    auto slashIndex{ faceStr.find_first_of('/') };
                    vertexIndices.push_back(std::stoi(faceStr.substr(0, slashIndex)));
                    textureCoordinateIndices.push_back(std::stoi(faceStr.substr(slashIndex + 1)));
                }
                else if (slashCount == 2)
                {
                    auto firstSlashIndex{ faceStr.find_first_of('/') };
                    auto lastSlashIndex{ faceStr.find_last_of('/') };
                    vertexIndices.push_back(std::stoi(faceStr.substr(0, firstSlashIndex)));
                    textureCoordinateIndices.push_back(std::stoi(std::string{
                        (faceStr.begin() + firstSlashIndex + 1),
                        (faceStr.begin() + lastSlashIndex) }));
                }
            }
            if (vertexIndices.size() != c_verticesPerFace)
            {
                SPDLOG_ERROR("OBJ file '{}' face specified {} vertices, expected {}",
                    objFilePath.string(), vertexIndices.size(), c_verticesPerFace);
                return nullptr;
            }
            if (vertexIndices.size() != textureCoordinateIndices.size())
            {
                SPDLOG_ERROR("OBJ file '{}' texture coordinate count does not match vertex count.",
                    objFilePath.string());
                return nullptr;
            }
            // Convert to 0-based indexing
            for (size_t i{ 0 }; i < vertexIndices.size(); ++i)
            {
                vertexIndices.at(i)--;
                textureCoordinateIndices.at(i)--;
            }
            faces.emplace_back(
                std::array<size_t, 3>{ vertexIndices.at(0), vertexIndices.at(1),
                    vertexIndices.at(2) },
                std::array<size_t, 3>{ textureCoordinateIndices.at(0),
                    textureCoordinateIndices.at(1), textureCoordinateIndices.at(2) },
                c_faceColors.at(faces.size() % c_faceColors.size()));
        }
    }
    std::shared_ptr<PngTexture> texture{ nullptr };
    if (textureFilePath)
    {
        SPDLOG_INFO("Loading PNG texture file '{}'...", textureFilePath.value().string());
        texture = PngTexture::FromPngFile(textureFilePath.value());
    }

    SPDLOG_INFO("Loaded OBJ file '{}' with {} vertices, {} texture coordinates, {} faces",
        objFilePath.string(), vertices.size(), textureCoordinates.size(), faces.size());
    return std::make_shared<Mesh>(vertices, textureCoordinates, faces, texture);
}

std::shared_ptr<Mesh> Mesh::Cube()
{
    std::vector<Vector3> vertices{
        // 'front' quad (negative Z)
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{ -0.5 }, FixedUnit{ -0.5 } }, // 0
        Vector3{ FixedUnit{  0.5 }, FixedUnit{ -0.5 }, FixedUnit{ -0.5 } }, // 1
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{  0.5 }, FixedUnit{ -0.5 } }, // 2
        Vector3{ FixedUnit{  0.5 }, FixedUnit{  0.5 }, FixedUnit{ -0.5 } }, // 3
        // 'back' quad (positive Z)
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{ -0.5 }, FixedUnit{  0.5 } }, // 4
        Vector3{ FixedUnit{  0.5 }, FixedUnit{ -0.5 }, FixedUnit{  0.5 } }, // 5
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{  0.5 }, FixedUnit{  0.5 } }, // 6
        Vector3{ FixedUnit{  0.5 }, FixedUnit{  0.5 }, FixedUnit{  0.5 } }, // 7
    };

    std::vector<MeshFace> faces{
        // FRONT
        MeshFace{ { 1, 0, 2 } },
        MeshFace{ { 1, 2, 3 } },
        // BACK
        MeshFace{ { 7, 6, 4 } },
        MeshFace{ { 7, 4, 5 } },
        // BOTTOM
        MeshFace{ { 3, 2, 6 } },
        MeshFace{ { 3, 6, 7 } },
        // TOP
        MeshFace{ { 5, 4, 0 } },
        MeshFace{ { 5, 0, 1 } },
        // LEFT
        MeshFace{ { 0, 4, 6 } },
        MeshFace{ { 0, 6, 2 } },
        // RIGHT
        MeshFace{ { 5, 1, 3 } },
        MeshFace{ { 5, 3, 7 } },
    };
    return std::make_shared<Mesh>(
        vertices,
        std::vector<Vector2>{},
        faces
    );
}

std::shared_ptr<Mesh> Mesh::AdjoiningTriangles()
{
    std::vector<Vector3> vertices{
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{ -0.5 }, FixedUnit{ 0 } },
        Vector3{ FixedUnit{  0.5 }, FixedUnit{ -0.5 }, FixedUnit{ 0 } },
        Vector3{ FixedUnit{ -0.5 }, FixedUnit{  0.5 }, FixedUnit{ 0 } },
        Vector3{ FixedUnit{  0.5 }, FixedUnit{  0.5 }, FixedUnit{ 0 } },
    };
    std::vector<MeshFace> faces{
        MeshFace{ { 2, 1, 0 } },
        MeshFace{ { 2, 3, 1 } },
    };
    return std::make_shared<Mesh>(
        vertices,
        std::vector<Vector2>{},
        faces
    );
}
