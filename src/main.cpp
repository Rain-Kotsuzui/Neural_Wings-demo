#include "Engine/Engine.h"

#include "Game/Screen.h"

int main()
{
    // 1. 配置
    EngineConfig config;
    config.load("assets/config/engine_config.json");

    // 2. 注册
    auto factory = std::make_unique<ScreenFactory>();
    
    factory->Register(static_cast<int>(ScreenStateID::START), []()
                      { return std::make_unique<StartScreen>(); });
    factory->Register(static_cast<int>(ScreenStateID::MAIN_MENU), []()
                      { return std::make_unique<MainMenuScreen>(); });
    factory->Register(static_cast<int>(ScreenStateID::GAMEPlAY), []()
                      { return std::make_unique<GameplayScreen>(); });

    ScreenManager app(config, std::move(factory));

    app.Run();

    return 0;
}