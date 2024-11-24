#pragma once
#include "../Input.h"

struct Mesh;

struct Entity
{
    template<class T, typename... Params>
    void MakeChild(Params&&... args)
    {
        m_childEntities.push_back(std::move(std::make_unique<T>(args...)));
    }

    std::vector<std::unique_ptr<Entity>> const& GetChildren() const
    {
        return m_childEntities;
    }

    std::vector<std::shared_ptr<Mesh>> const& GetMeshes() const
    {
        return m_meshes;
    }

    Vector3 const& GetPosition() const
    {
        return m_position;
    }

    Vector3 const& GetRotation() const
    {
        return m_rotation;
    }

    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input)
    {
        for (const auto& childEntity : m_childEntities)
        {
            childEntity->Update(deltaTime, input);
        }
    }

protected:
    std::vector<std::unique_ptr<Entity>> m_childEntities;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    Vector3 m_position{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } };
    Vector3 m_rotation{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } };
};