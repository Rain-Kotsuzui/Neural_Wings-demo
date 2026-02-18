#pragma once
#include "Engine/Graphics/ShaderWrapper.h"
#include "Engine/Core/Components/Components.h"
#include <vector>

#define MAX_LIGHTS 16
class GameWorld;
class LightingManager
{
public:
    void Update(GameWorld &world);
    void UploadToShader(std::shared_ptr<ShaderWrapper> shader, const Vector3f &viewPos);

private:
    struct LightInfo
    {
        LightComponent *data;
        Vector3f worldPosition;
        Vector3f worldDirection;
    };

    std::vector<LightInfo> m_activeLights;
};