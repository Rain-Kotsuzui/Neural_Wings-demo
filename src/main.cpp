#include "Engine/Engine.h"

#include "Game/Screen/MainMenuScreen.h"
#include "Game/Screen/GameplayScreen.h"

int main()
{
    // 1. 启动配置
    EngineConfig config;
    config.load("assets/config/engine_config.json");

    // 2. 注册屏幕到工厂
    auto factory = std::make_unique<ScreenFactory>();
    factory->Register(ScreenState::MAIN_MENU, []()
                      { return std::make_unique<MainMenuScreen>(); });
    factory->Register(ScreenState::GAMEPLAY, []()
                      { return std::make_unique<GameplayScreen>(); });

    // 3. 创建 ScreenManager
    ScreenManager app(config, std::move(factory));

    // 4. 启动应用
    app.Run();

    return 0;
}