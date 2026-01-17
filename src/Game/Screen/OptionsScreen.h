#pragma once
#include "Engine/System/Screen/GameScreen.h"

class OptionsScreen : public GameScreen
{
public:
    OptionsScreen();
    virtual ~OptionsScreen() = default;

    void OnEnter() override;
    void FixedUpdate(float fixedDeltaTime) override;
    void Update(float deltaTime) override;
    void Draw() override;
    void OnExit() override;
    ScreenState GetNextScreenState() const override;

private:
    ScreenState m_nextScreenState;

    // 设置选项
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    bool fullscreen;
};
