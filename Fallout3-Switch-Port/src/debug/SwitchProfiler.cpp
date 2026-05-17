#include "SwitchProfiler.h"
#include <cstdio>

SwitchProfiler& SwitchProfiler::Instance()
{
    static SwitchProfiler prof;
    return prof;
}

void SwitchProfiler::BeginFrame()
{
    m_frameStartTick = armGetSystemTick();
}

void SwitchProfiler::EndFrame()
{
    u64 endTick     = armGetSystemTick();
    u64 elapsed     = endTick - m_frameStartTick;
    m_accumulated  += elapsed;
    ++m_sampleCount;

    if (m_sampleCount >= 60)
    {
        float avgTicks = (float)m_accumulated / (float)m_sampleCount;
        m_frameMs      = avgTicks / (float)m_perfFreq * 1000.0f;
        m_fps          = 1.0f / (avgTicks / (float)m_perfFreq);
        printf("Profiler: %.1f FPS (%.2f ms)\n", m_fps, m_frameMs);
        m_accumulated = 0;
        m_sampleCount = 0;
    }
}