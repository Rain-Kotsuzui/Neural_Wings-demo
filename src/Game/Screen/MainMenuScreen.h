#pragma once
#include "Engine/System/Screen/GameScreen.h" // 必须包含引擎提供的接口

class MainMenuScreen : public GameScreen {
public:
    MainMenuScreen();
    virtual ~MainMenuScreen() = default; // 使用默认的虚析构函数

    // --- 实现 IGameScreen 接口 ---

    void OnEnter() override;
    void FixedUpdate(float fixedDeltaTime) override;
    void Update(float deltaTime) override;
    void Draw() override;
    void OnExit() override;
    ScreenState GetNextScreenState() const override;

private:
    // 用于存储本帧用户操作所请求的下一个屏幕状态
    ScreenState m_nextScreenState;
};