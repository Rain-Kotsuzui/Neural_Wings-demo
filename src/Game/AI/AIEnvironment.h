#pragma once
#if !defined(PLATFORM_WEB)
#include <vector>
#include <string>
#include <memory>
#include "Engine/Core/GameWorld.h"
#include "Engine/Graphics/Renderer.h"

struct StepResult
{
    std::vector<float> image_data;
    std::vector<float> sensors;
    float reward;
    bool done;
};

class AIEnvironment
{
public:
    AIEnvironment();
    ~AIEnvironment();
    void Init(const std::string &scenePath);

    void Reset();
    StepResult Step(const std::vector<float> &action);

private:
    GameWorld *m_gameWorld;
    RenderTexture2D m_aiFbo;
    float m_currentTime = 0.0f;

    float CalculateReward(GameObject *player, GameObject *target);
    std::vector<float> GetSenesors();
};
#endif