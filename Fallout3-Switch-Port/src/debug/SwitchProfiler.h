#pragma once

#include <switch.h>

class SwitchProfiler
{
public:
    static SwitchProfiler& Instance();

    void BeginFrame();
    void EndFrame();

    float GetFPS()      const { return m_fps; }
    float GetFrameMs()  const { return m_frameMs; }

private:
    SwitchProfiler() = default;

    u64   m_perfFreq       = armGetSystemTickFreq();
    u64   m_frameStartTick = 0;
    float m_fps            = 0.0f;
    float m_frameMs        = 0.0f;
    int   m_sampleCount    = 0;
    u64   m_accumulated    = 0;
};