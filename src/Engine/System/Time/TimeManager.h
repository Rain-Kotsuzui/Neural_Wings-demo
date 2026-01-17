#pragma once

class TimeManager {
public:
    TimeManager();
    void Tick();

    // 获取可变的 delta time (用于渲染和非物理逻辑)
    float GetDeltaTime() const;

    // 获取固定的 delta time (用于物理和网络)
    float GetFixedDeltaTime() const;

private:
    double m_lastFrameTime;
    float m_deltaTime;
    const float m_fixedDeltaTime;
};