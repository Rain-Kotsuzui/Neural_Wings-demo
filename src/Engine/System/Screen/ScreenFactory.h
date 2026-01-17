#pragma once
#include "GameScreen.h"
#include <memory>
#include <functional>
#include <unordered_map>

class ScreenFactory {
public:
    using ScreenCreator = std::function<std::unique_ptr<GameScreen>()>;

    void Register(ScreenState state, ScreenCreator creator);

    std::unique_ptr<GameScreen> Create(ScreenState state);

private:
    std::unordered_map<ScreenState, ScreenCreator> m_creators;
};