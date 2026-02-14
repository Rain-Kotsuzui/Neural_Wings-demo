#pragma once
#include "IComponent.h"
#include "raylib.h"
#include <unordered_map>
#include <string>
#include <iostream>

struct AudioClip
{
    Sound sound;

    bool is3D = true;
    bool isLooping = false;
    float baseVolume = 1.0f;
    float basePitch = 1.0f;

    float minDis = 5.0f;  // 最大音量半径
    float maxDis = 50.0f; // 最小音量半径

    bool isPlaying = false;

    AudioClip() : sound({0})
    {
    }

    // 多重播放
    std::vector<Sound> aliases;
    int nextAliasIndex = 0;
    bool isMulti = false;

    void SetupMultiVoice(int voiceCount = 10)
    {
        isMulti = true;
        aliases.clear();
        for (int i = 0; i < voiceCount; ++i)
        {
            aliases.push_back(LoadSoundAlias(sound));
        }
    }
};

class AudioComponent : public IComponent
{
public:
    AudioComponent() = default;
    ~AudioComponent()
    {
        for (auto &[name, clip] : audioClips)
        {
            for (auto &s : clip.aliases)
                UnloadSoundAlias(s);
        }
    }
    std::unordered_map<std::string, AudioClip> audioClips;

    void Play(const std::string &name)
    {
        if (!audioClips.count(name))
        {
            std::cerr << "[AudioComponent]: No audio clip named '" << name << "' found!" << std::endl;
            return;
        }
        auto &clip = audioClips[name];

        if (audioClips.count(name))
        {
            auto &clip = audioClips[name];
            if (clip.isMulti)
            {
                Sound &targetSound = clip.aliases[clip.nextAliasIndex];
                PlaySound(targetSound);
                clip.nextAliasIndex = (clip.nextAliasIndex + 1) % clip.aliases.size();
            }
            else
                PlaySound(clip.sound);
            clip.isPlaying = true;
        }
    }
    void Stop(const std::string &name)
    {
        if (audioClips.count(name))
        {
            StopSound(audioClips[name].sound);
            audioClips[name].isPlaying = false;
        }
    }

    void SetVolume(const std::string &name, float vol)
    {
        if (audioClips.count(name))
            audioClips[name].baseVolume = vol;
    }

    void SetPitch(const std::string &name, float pitch)
    {
        if (audioClips.count(name))
        {
            audioClips[name].basePitch = pitch;
            SetSoundPitch(audioClips[name].sound, pitch);
        }
    }
};