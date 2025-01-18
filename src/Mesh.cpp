#include <pch.h>
#include "Mesh.h"

std::shared_ptr<Mesh> Mesh::FromObjFile(std::filesystem::path filePath)
{
    // TODO: Load from file
    // return std::make_shared<Mesh>(
    //     std::vector<glm::vec4>{},      // vertices
    //     std::vector<MeshFace>{},       // faces
    //     glm::vec3{ 0.0f, 0.0f, 0.0f }, // rotation
    //     glm::vec3{ 0.0f, 0.0f, 0.0f }, // scale
    //     glm::vec3{ 0.0f, 0.0f, 0.0f }  // translation
    // );
    return nullptr;
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
        faces
    );
}
