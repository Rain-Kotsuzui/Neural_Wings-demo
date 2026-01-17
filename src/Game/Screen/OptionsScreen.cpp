#include "OptionsScreen.h"
#include "raylib.h"
#include "raygui.h"

OptionsScreen::OptionsScreen()
    : m_nextScreenState(ScreenState::NONE), masterVolume(0.5f), musicVolume(0.5f), sfxVolume(0.5f), fullscreen(false)
{
}

void OptionsScreen::OnEnter()
{
    // 初始化设置
}

void OptionsScreen::OnExit()
{
    // 清理资源
}

void OptionsScreen::FixedUpdate(float fixedDeltaTime)
{
    // 设置界面不需要物理更新
}

void OptionsScreen::Update(float deltaTime)
{
    // 重置状态请求
    m_nextScreenState = ScreenState::NONE;
}

void OptionsScreen::Draw()
{
    ClearBackground(BLACK);

    // 标题
    const char *title = "OPTIONS";
    int titleFontSize = 60;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, 100, titleFontSize, RAYWHITE);

    // 按钮布局
    float buttonWidth = 220;
    float buttonHeight = 50;
    float centerX = GetScreenWidth() / 2.0f;
    float startY = 250;
    float spacing = 15.0f;

    // 音量滑块
    GuiLabel({centerX - buttonWidth / 2, startY, buttonWidth, buttonHeight}, "Master Volume");
    GuiSliderBar({centerX - buttonWidth / 2, startY + 40, buttonWidth, 20},
                 NULL, NULL, &masterVolume, 0.0f, 1.0f);

    GuiLabel({centerX - buttonWidth / 2, startY + 80, buttonWidth, buttonHeight}, "Music Volume");
    GuiSliderBar({centerX - buttonWidth / 2, startY + 120, buttonWidth, 20},
                 NULL, NULL, &musicVolume, 0.0f, 1.0f);

    GuiLabel({centerX - buttonWidth / 2, startY + 160, buttonWidth, buttonHeight}, "SFX Volume");
    GuiSliderBar({centerX - buttonWidth / 2, startY + 200, buttonWidth, 20},
                 NULL, NULL, &sfxVolume, 0.0f, 1.0f);

    // 全屏开关
    GuiCheckBox({centerX - buttonWidth / 2, startY + 240, 20, 20}, "Fullscreen", &fullscreen);
    if (fullscreen != IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    // 返回按钮
    if (GuiButton({centerX - buttonWidth / 2, startY + 300, buttonWidth, buttonHeight}, "Back"))
    {
        m_nextScreenState = ScreenState::MAIN_MENU;
    }
}

ScreenState OptionsScreen::GetNextScreenState() const
{
    return m_nextScreenState;
}
