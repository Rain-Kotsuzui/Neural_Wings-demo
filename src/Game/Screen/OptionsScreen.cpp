#include "OptionsScreen.h"
#include "raylib.h"
// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"

OptionsScreen::OptionsScreen()
    : m_nextScreenState(static_cast<int>(ScreenStateID::NONE)){}
void OptionsScreen::OnEnter(){}
void OptionsScreen::OnExit(){}
void OptionsScreen::FixedUpdate(float fixedDeltaTime){}

void OptionsScreen::Update(float deltaTime)
{
    m_nextScreenState = static_cast<int>(ScreenStateID::NONE);
}

void OptionsScreen::Draw()
{
    ClearBackground(BLACK);

    const char *title = "OPTIONS";
    int titleFontSize = 60;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, 100, titleFontSize, RAYWHITE);

    float buttonWidth = 220;
    float buttonHeight = 50;
    float centerX = GetScreenWidth() / 2.0f;
    float spacing = 15.0f;

    if (GuiButton({centerX - buttonWidth / 2, static_cast<float>(GetScreenHeight()) - 100, buttonWidth, buttonHeight}, "Back"))
    {
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }
}

int OptionsScreen::GetNextScreenState() const
{
    return m_nextScreenState;
}

int OptionsScreen::GetScreenState() const
{
    return static_cast<int>(ScreenStateID::OPTIONS);
}
