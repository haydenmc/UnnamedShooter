#pragma once
#include "../Input.h"

struct Mesh;

struct Entity
{
    template<class T, typename... Params>
    T* MakeChild(Params&&... args)
    {
        std::unique_ptr<T> instance{ std::make_unique<T>(args...) };
        T* instancePtr{ instance.get() };
        m_childEntities.push_back(std::move(instance));
        return instancePtr;
    }

    std::vector<std::unique_ptr<Entity>> const& GetChildren() const
    {
        return m_childEntities;
    }

    std::vector<std::shared_ptr<Mesh>> const& GetMeshes() const
    {
        return m_meshes;
    }

    Eigen::Vector3f const& GetPosition() const
    {
        return m_position;
    }

    void SetPosition(Eigen::Vector3f const& position)
    {
        m_position = position;
    }

    Eigen::Vector3f const& GetRotation() const
    {
        return m_rotation;
    }

    void SetRotation(Eigen::Vector3f const& rotation)
    {
        m_rotation = rotation;
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
    Eigen::Vector3f m_position{ 0.0f, 0.0f, 0.0f };
    Eigen::Vector3f m_rotation{ 0.0f, 0.0f, 0.0f };
};