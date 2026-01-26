#include "OptionsScreen.h"
#include "raylib.h"
#include "Engine/System/Screen/ScreenManager.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include <cstdio>
OptionsScreen::OptionsScreen()
    : m_nextScreenState(static_cast<int>(ScreenStateID::NONE))
{
}
OptionsScreen::~OptionsScreen()
{
    OnExit();
}
void OptionsScreen::OnEnter()
{
    m_currentConfig.load("assets/config/engine_config.json");

    m_modifiedConfig = m_currentConfig;

    if (screenManager && screenManager->GetUILayer())
    {
        screenManager->GetUILayer()->SetVisible(true);
        screenManager->GetUILayer()->LoadRoute("options");
    }
}
void OptionsScreen::OnExit()
{
    // 不再自动应用设置，只有点击保存按钮时才应用
    if (screenManager && screenManager->GetUILayer())
    {
        screenManager->GetUILayer()->SetVisible(false);
    }
}
void OptionsScreen::FixedUpdate(float fixedDeltaTime) {}

void OptionsScreen::Update(float deltaTime)
{
    m_nextScreenState = static_cast<int>(ScreenStateID::NONE);

    // 检查 Vue 路由是否已变化
    if (screenManager && screenManager->GetUILayer())
    {
        std::string currentRoute = screenManager->GetUILayer()->GetCurrentRoute();

        if (currentRoute == "#/menu")
        {
            m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
            return;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }

    // 检查 Vue 中的设置是否被保存
    if (screenManager && screenManager->GetUILayer())
    {
        ApplyVueSettings();
    }
}

void OptionsScreen::Draw()
{
    if (screenManager && screenManager->GetUILayer())
    {
        screenManager->GetUILayer()->Draw();
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

#include <fstream>
void OptionsScreen::SaveConfig()
{
    json configJson;
    m_modifiedConfig.toJson(configJson);

    std::ofstream o("assets/config/engine_config.json");
    if (o.is_open())
    {
        o << configJson.dump(4);
        o.close();
        printf("[OptionsScreen] Config saved to engine_config.json\n");
    }
}

void OptionsScreen::ApplyVueSettings()
{
    // Check if save is requested
    std::string saveRequestedStr = screenManager->GetUILayer()->GetAppState("settingsSaveRequested");

    if (saveRequestedStr == "true")
    {
        // Read settings values
        std::string fullscreenStr = screenManager->GetUILayer()->GetAppState("fullscreen");
        std::string resolutionStr = screenManager->GetUILayer()->GetAppState("resolution");
        std::string fpsStr = screenManager->GetUILayer()->GetAppState("targetFPS");

        // Parse resolution string (e.g. "1920x1080")
        size_t xPos = resolutionStr.find('x');
        if (xPos != std::string::npos)
        {
            try
            {
                int width = std::stoi(resolutionStr.substr(0, xPos));
                int height = std::stoi(resolutionStr.substr(xPos + 1));

                // Update configuration
                m_modifiedConfig.screenWidth = width;
                m_modifiedConfig.screenHeight = height;
            }
            catch (...)
            {
                printf("[OptionsScreen] Failed to parse resolution: %s\n", resolutionStr.c_str());
            }
        }

        // Parse fullscreen setting
        m_modifiedConfig.fullScreen = (fullscreenStr == "true");

        // Parse FPS
        try
        {
            m_modifiedConfig.targetFPS = static_cast<float>(std::stoi(fpsStr));
        }
        catch (...)
        {
            printf("[OptionsScreen] Failed to parse FPS: %s\n", fpsStr.c_str());
        }

        printf("[OptionsScreen] Settings saved and applied: %dx%d, fullscreen=%s, fps=%.0f\n",
               m_modifiedConfig.screenWidth,
               m_modifiedConfig.screenHeight,
               m_modifiedConfig.fullScreen ? "true" : "false",
               m_modifiedConfig.targetFPS);

        // Save to file immediately
        SaveConfig();

        // Apply settings to game engine immediately
        if (screenManager)
        {
            screenManager->ApplySettings(m_modifiedConfig);
        }

        // Reset save request flag
        screenManager->GetUILayer()->ExecuteScript("window.vueAppState.settingsSaveRequested = false;");

        printf("[OptionsScreen] Settings applied to engine\n");
    }
}
