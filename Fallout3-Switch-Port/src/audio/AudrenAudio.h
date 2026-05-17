#pragma once

#include <switch.h>
#include <audren.h>

class AudrenAudio
{
public:
    AudrenAudio();
    ~AudrenAudio();

    bool Initialize(size_t heapSize, int sampleRate, int channels);
    void Update();
    void Shutdown();

private:
    AudioDriver           m_driver = {};
    AudioDriverWaveBuf*   m_waveBufs = nullptr;
    int                   m_maxVoices = 0;
    bool                  m_initialized = false;

    static constexpr int MAX_WAVEBUFS_PER_VOICE = 4;
};