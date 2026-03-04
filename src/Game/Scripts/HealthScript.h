#pragma once
#include "Engine/Core/Components/Components.h"
#include "Engine/Core/Events/EventManager.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class HealthScript : public IScriptableComponent
{
public:
    float maxHP = 100.0f;
    float currentHP = 100.0f;
    float m_hitFlashTimer = 0.0f;
    float m_flashDuration = 0.2f;
    Vector4f m_defaultColor = Vector4f(255.0f, 255.0f, 255.0f, 255.0f);
    Vector4f m_hitFlashColor = Vector4f(255.0f, 0.0f, 0.0f, 255.0f);

    void Initialize(const json &data) override
    {
        m_flashDuration = data.value("flashDuration", 0.2f);
        maxHP = data.value("maxHP", 100.0f);
        currentHP = maxHP;
    }

    void OnCreate() override;
    void OnWake() override;
    void OnDestroy() override;
    void OnUpdate(float dt) override;

private:
    Subscription_ID m_subID = 0;
};