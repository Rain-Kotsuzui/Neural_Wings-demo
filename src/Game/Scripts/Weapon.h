#pragma once
#include "Engine/Core/Components/Components.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class BulletScript : public IScriptableComponent
{
public:
    float lifeTime = 2.0f;
    float timer = 0.0f;
    float m_fireTimer = 0.0f;
    BulletScript() = default;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnWake() override;

    Vector3f m_velocity = Vector3f(0, 0, 0);
};

class WeaponScript : public IScriptableComponent
{
public:
    WeaponScript() = default;
    void OnUpdate(float deltaTime) override;

    float m_fireTimer = 0.0f;
};