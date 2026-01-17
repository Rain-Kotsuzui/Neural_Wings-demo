#pragma once

#include "Engine/Utils/Timer.h" 
#include <vector>
#include <memory>       
#include <functional>   

class TimerManager {
public:
    /**
     * @brief 在主循环的每一帧调用，用于更新所有活动的计时器。
     * @param deltaTime 从上一帧到现在所经过的时间。
     */
    void Update(float deltaTime);

    /**
     * @brief 添加一个新的计时器任务。这是创建计时器的主要方式。
     * @param duration 计时器的总时长（秒）。
     * @param callback 时间到达后要执行的回调函数。
     * @param repeat 计时器完成后是否自动重复。默认为 false。
     * @return 返回一个指向新创建的 Timer 的原始指针。
     *         可以保留此指针以手动暂停、恢复或重置计时器，但不需要手动删除它。
     */
    Timer* AddTimer(float duration, std::function<void()> callback, bool repeat = false);

private:
    std::vector<std::unique_ptr<Timer>> m_timers;
};