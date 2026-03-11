#if !defined(PLATFORM_WEB)
#include "AIEnvironment.h"
#include "Engine/Core/Components/Components.h"
#include "rlgl.h"
#include <random>

void AIEnvironment::Init(const std::string &scenePath)
{
}

void AIEnvironment::Reset()
{
    m_currentTime = 0.0f;
    m_gameWorld->Reset();
    auto *player = m_gameWorld->GetEntitiesByTag("Player")[0];
    auto *target = m_gameWorld->GetEntitiesByTag("Enemy")[0];

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    player->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));
    target->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));

    player->GetComponent<RigidbodyComponent>().velocity = Vector3f(dis(gen) * 100, dis(gen) * 100, dis(gen) * 100);
    m_gameWorld->UpdateTransforms();
}
StepResult AIEnvironment::Step(const std::vector<float> &action)
{
    auto &input = m_gameWorld->GetInputManager();
    // input.SetKeyState("Pitch", action[0]);

    float dt = 1.0f / 60.0f;
    m_currentTime += dt;
    m_gameWorld->FixedUpdate(dt);
    m_gameWorld->Update(dt);

    // TODO: render AI view
}

#endif