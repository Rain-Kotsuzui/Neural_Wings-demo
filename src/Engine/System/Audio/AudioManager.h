#pragma once
#include "Engine/Graphics/Camera/mCamera.h"
#include "Engine/Core/Components/AudioComponent.h"
#include <string>
class GameWorld;
class AudioManager
{
public:
    AudioManager() = default;
    ~AudioManager() = default;

    void Update(GameWorld &world, const mCamera &camera);
    void Play2D(const std::string &path, float volume = 1.0f, float pitch = 1.0f);

private:
    void Spatialize(AudioClip &clip, const Vector3f &sourceWorldPos, const mCamera &listener);
};