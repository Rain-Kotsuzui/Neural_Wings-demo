#include "ScreenFactory.h"

void ScreenFactory::Register(ScreenState state, ScreenCreator creator) {
    m_creators[state] = creator;
}

std::unique_ptr<GameScreen> ScreenFactory::Create(ScreenState state) {
    auto it = m_creators.find(state);
    if (it != m_creators.end()) {
        return (it->second)();
    }
    return nullptr;
}