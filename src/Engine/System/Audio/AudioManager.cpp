#include "AudioManager.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/System/Resource/ResourceManager.h"
#include "Engine/Math/Math.h"
#include "Engine/Core/GameWorld.h"

void AudioManager::Update(GameWorld &world, const mCamera &camera)
{
    auto entities = world.GetEntitiesWith<AudioComponent, TransformComponent>();
    for (auto *entity : entities)
    {
        auto &audioComp = entity->GetComponent<AudioComponent>();
        auto &tf = entity->GetComponent<TransformComponent>();
        Vector3f sourcePos = tf.GetWorldPosition();

        for (auto &[name, clip] : audioComp.audioClips)
        {
            if (!clip.isPlaying)
                continue;
            if (clip.is3D)
                Spatialize(clip, sourcePos, camera);
            else
            {
                SetSoundVolume(clip.sound, clip.baseVolume);
                SetSoundPan(clip.sound, 0.5f);
            }

            bool actuallyPlaying = false;
            if (clip.isMulti)
            {
                for (auto &s : clip.aliases)
                {
                    if (IsSoundPlaying(s))
                    {
                        actuallyPlaying = true;
                        break;
                    }
                }
            }
            else
                actuallyPlaying = IsSoundPlaying(clip.sound);

            if (clip.isLooping && !actuallyPlaying)
            {
                PlaySound(clip.isMulti ? clip.aliases[0] : clip.sound);
                actuallyPlaying = true;
            }

            if (!actuallyPlaying)
            {
                clip.isPlaying = false;
            }
        }
    }
}
void AudioManager::Spatialize(AudioClip &clip, const Vector3f &sourcePos, const mCamera &listener)
{
    Vector3f listenerPos = listener.Position();
    Vector3f diff = sourcePos - listenerPos;
    float dist = diff.Length();

    float finalVolume = 0.0f;
    if (dist < clip.minDis)
    {
        finalVolume = clip.baseVolume;
    }
    else if (dist < clip.maxDis)
    {
        finalVolume = clip.baseVolume * (1.0f - (dist - clip.minDis) / (clip.maxDis - clip.minDis));
    }
    SetSoundVolume(clip.sound, finalVolume);

    if (dist > 0.001f)
    {
        Vector3f dirToSource = diff.Normalized();
        Vector3f listenerRight = listener.Right();

        float dot = dirToSource * listenerRight;
        float pan = (dot + 1.0f) * 0.5f;
        SetSoundPan(clip.sound, pan);
        if (clip.isMulti)
        {
            for (auto &s : clip.aliases)
            {
                SetSoundVolume(s, finalVolume);
                SetSoundPan(s, pan);
            }
        }
    }
}

void AudioManager::Play2D(const std::string &path, float volume, float pitch)
{
    // TODO
}