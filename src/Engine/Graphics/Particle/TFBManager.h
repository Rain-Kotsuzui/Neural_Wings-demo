#pragma once
#include "Engine/Graphics/ShaderWrapper.h"
#include "GPUParticleBuffer.h"

class TFBManager
{
public:
    TFBManager() = default;
    void Simulate(Texture2D &dataTex, int maxParticles, ShaderWrapper &shader, GPUParticleBuffer &buffer, int count, float dt);
};