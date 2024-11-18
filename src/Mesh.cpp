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
    std::vector<glm::vec4> vertices{
        glm::vec4{  1.0f, -1.0f, -1.0f,  1.0f },
        glm::vec4{  1.0f, -1.0f,  1.0f,  1.0f },
        glm::vec4{ -1.0f, -1.0f,  1.0f,  1.0f },
        glm::vec4{ -1.0f, -1.0f, -1.0f,  1.0f },
        glm::vec4{  1.0f,  1.0f, -1.0f,  1.0f },
        glm::vec4{  1.0f,  1.0f,  1.0f,  1.0f },
        glm::vec4{ -1.0f,  1.0f,  1.0f,  1.0f },
        glm::vec4{ -1.0f,  1.0f, -1.0f,  1.0f },
    };
    std::vector<MeshFace> faces{
        // TOP
        MeshFace{ { 0, 1, 3 } },
        MeshFace{ { 1, 2, 3 } },
        // BACK
        MeshFace{ { 5, 1, 2 } },
        MeshFace{ { 5, 6, 2 } },
        // BOTTOM
        MeshFace{ { 4, 5, 6 } },
        MeshFace{ { 4, 6, 7 } },
        // FRONT
        MeshFace{ { 7, 4, 0 } },
        MeshFace{ { 7, 0, 3 } },
        // LEFT
        MeshFace{ { 3, 2, 6 } },
        MeshFace{ { 6, 7, 3 } },
        // RIGHT
        MeshFace{ { 1, 0, 4 } },
        MeshFace{ { 1, 4, 5 } },
    };
    return std::make_shared<Mesh>(
        vertices,
        faces
    );
}
