#include "TimerManager.h"

void TimerManager::Update(float deltaTime) {
    for (int i = m_timers.size() - 1; i >= 0; --i) {
        
        // 如果计时器指针有效
        if (m_timers[i]) {
            // 更新
            m_timers[i]->Tick(deltaTime);
            if (m_timers[i]->IsFinished()) {
                // 如果已完成，就从列表中移除这个计时器。
                m_timers.erase(m_timers.begin() + i);
            }
        }
    }
}

Timer* TimerManager::AddTimer(float duration, std::function<void()> callback, bool repeat) {
    auto newTimer = std::make_unique<Timer>(duration, callback, repeat);
    Timer* rawPtr = newTimer.get();
    m_timers.push_back(std::move(newTimer));
    return rawPtr;
}