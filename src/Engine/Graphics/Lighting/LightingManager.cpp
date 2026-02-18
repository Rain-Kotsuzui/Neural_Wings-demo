#include "LightingManager.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/Core/GameWorld.h"

void LightingManager::Update(GameWorld &world)
{
    m_activeLights.clear();
    auto entities = world.GetEntitiesWith<LightComponent, TransformComponent>();
    for (auto *entity : entities)
    {
        if (m_activeLights.size() >= MAX_LIGHTS)
            break;

        auto &light = entity->GetComponent<LightComponent>();
        auto &tf = entity->GetComponent<TransformComponent>();

        LightInfo info;
        info.data = &light;
        info.worldPosition = tf.GetWorldPosition();
        if (light.type == LightType::Directional)
        {
            info.worldDirection = tf.GetWorldRotation() * (light.direction).Normalized();
        }
        else
        {
            info.worldDirection = light.direction;
        }
        m_activeLights.push_back(info);
    }
}
void LightingManager::UploadToShader(std::shared_ptr<ShaderWrapper> shader, const Vector3f &viewPos)
{
    if (!shader || !shader->IsValid())
        return;
    shader->SetInt("lightCounts", (int)m_activeLights.size());
    shader->SetVec3("viewPos", viewPos);

    for (int i = 0; i < m_activeLights.size(); ++i)
    {
        std::string base = "lights[" + std::to_string(i) + "]";
        const auto &info = m_activeLights[i];

        shader->SetInt(base + ".type", (int)info.data->type);
        shader->SetVec3(base + ".position", info.worldPosition);
        shader->SetVec3(base + ".direction", info.worldDirection);
        shader->SetVec3(base + ".color", info.data->color / 255.0f);
        shader->SetFloat(base + ".intensity", info.data->intensity);
        shader->SetFloat(base + ".range", info.data->range);
    }
}