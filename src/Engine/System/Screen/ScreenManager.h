#pragma once
#include "GameScreen.h"
#include "ScreenFactory.h"
#include "Engine/Config/EngineConfig.h"
#include "Engine/System/Time/TimeManager.h"
#include "Engine/UI/UI.h"
#include "Engine/Network/Client/NetworkClient.h"
#include "Engine/Network/Client/ClientIdentity.h"

#include <memory>

class ScreenManager
{
public:
    ScreenManager(const EngineConfig &config, std::unique_ptr<ScreenFactory> factory);
    ~ScreenManager();
    // void Run();

    void ApplySettings(const EngineConfig &config);
    const EngineConfig &GetActiveConfig() const;
    UILayer *GetUILayer();

    /// Global NetworkClient shared across all screens.
    std::shared_ptr<NetworkClient> GetNetworkClient() { return m_networkClient; }
    NetworkClient &GetNetworkClientRef() { return *m_networkClient; }

    bool UpdateFrame();
    void Shutdown();

private:
    void ChangeScreen(int newState);

    std::unique_ptr<GameScreen> m_currentScreen;
    std::unique_ptr<ScreenFactory> m_factory;

    std::unique_ptr<UILayer> m_uiLayer;

    /// Persistent network client — lives as long as ScreenManager.
    std::shared_ptr<NetworkClient> m_networkClient;
    ClientIdentity m_clientIdentity;

    TimeManager m_timeManager;
    float m_accumulator;

    // Track the last applied runtime configuration so screens can sync from
    // the actual window state instead of only the config file on disk.
    EngineConfig m_activeConfig;
};
