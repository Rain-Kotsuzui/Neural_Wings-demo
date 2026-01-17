#pragma once
#include "Config.h"
#include "Engine/System/Screen/GameScreen.h"
#include <string>

struct EngineConfig : public Config
{
    int screenWidth = 1280;
    int screenHeight = 720;
    std::string windowTitle = "Default Engine Window";

    int targetFPS = 60;

    ScreenState initialScreen = ScreenState::MAIN_MENU;

protected:
    ScreenState ScreenStateFromString(const std::string &s)
    {
        if (s == "MAIN_MENU")
            return ScreenState::MAIN_MENU;
        if (s == "GAMEPLAY")
            return ScreenState::GAMEPLAY;
        if (s == "NONE")
            return ScreenState::NONE;
        if (s == "EXIT")
            return ScreenState::EXIT;
        return ScreenState::NONE;
    }
    void ParseJson(const json &configJson) override
    {
        this->screenWidth = configJson.at("window").value("width", this->screenWidth);
        this->screenHeight = configJson.at("window").value("height", this->screenHeight);
        this->windowTitle = configJson.at("window").value("title", this->windowTitle);
        this->targetFPS = configJson.at("performance").value("targetFPS", this->targetFPS);

        std::string initialScreenStr = configJson.at("application").value("initialScreen", "MAIN_MENU");
        this->initialScreen = ScreenStateFromString(initialScreenStr);
    }
};
