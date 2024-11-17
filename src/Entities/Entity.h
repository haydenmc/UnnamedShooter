#pragma once
#include "../Input.h"

struct Entity
{
    template<class T, typename... Params>
    void MakeChild(Params&&... args)
    {
        m_childEntities.push_back(std::move(std::make_unique<T>(args)));
    }

    virtual void Update(std::chrono::microseconds deltaTime, InputState const& input)
    {
        for (const auto& childEntity : m_childEntities)
        {
            childEntity->Update(deltaTime, input);
        }
    }

private:
    std::vector<std::unique_ptr<Entity>> m_childEntities;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
};