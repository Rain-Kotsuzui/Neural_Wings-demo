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

    int initialScreen = SCREEN_STATE_NONE;

protected:
    void ParseJson(const json &configJson) override
    {
        this->screenWidth = configJson.at("window").value("width", this->screenWidth);
        this->screenHeight = configJson.at("window").value("height", this->screenHeight);
        this->windowTitle = configJson.at("window").value("title", this->windowTitle);
        this->targetFPS = configJson.at("performance").value("targetFPS", this->targetFPS);
        this->initialScreen = SCREEN_STATE_START;
    }
};
