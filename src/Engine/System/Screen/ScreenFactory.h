#pragma once
#include "GameScreen.h"
#include <memory>
#include <functional>
#include <unordered_map>

class ScreenFactory {
public:
    using ScreenCreator = std::function<std::unique_ptr<GameScreen>()>;

    void Register(int state, ScreenCreator creator);

    std::unique_ptr<GameScreen> Create(int state);

private:
    std::unordered_map<int, ScreenCreator> m_creators;
};