#include "AudrenAudio.h"
#include <cstdio>
#include <cstring>
#include <malloc.h>

AudrenAudio::AudrenAudio()  = default;
AudrenAudio::~AudrenAudio() { Shutdown(); }

bool AudrenAudio::Initialize(size_t heapSize, int sampleRate, int channels)
{
    if (m_initialized) return true;

    AudioRendererConfig config;
    audrenGetDefaultConfig(&config, AudioRendererOutputMode_Pcm, channels, sampleRate, nullptr);

    Result rc = audrenInitialize(&config, &m_driver);
    if (R_FAILED(rc))
    {
        fprintf(stderr, "Audren: audrenInitialize failed: 0x%08X\n", rc);
        return false;
    }

    void* heap = memalign(0x1000, heapSize);
    if (!heap)
    {
        fprintf(stderr, "Audren: failed to allocate audio heap\n");
        audrenExit();
        return false;
    }
    audrenAttachWorkBuffer(&m_driver, heap, heapSize);

    m_maxVoices = 24;
    const int totalWaves = m_maxVoices * MAX_WAVEBUFS_PER_VOICE;
    m_waveBufs = new AudioDriverWaveBuf[totalWaves];
    memset(m_waveBufs, 0, sizeof(AudioDriverWaveBuf) * totalWaves);

    for (int i = 0; i < m_maxVoices; ++i)
    {
        audrvVoiceInit(&m_driver, i, channels, sampleRate, AUDREN_PCM_FORMAT_PCM_INT16);
        audrvVoiceSetVolume(&m_driver, i, 0, 1.0f);
        audrvVoiceSetDestinationMix(&m_driver, i, AUDREN_MIX_UNIT_0, 0);
        audrvVoiceStart(&m_driver, i);
    }

    rc = audrenStartAudioRenderer();
    if (R_FAILED(rc))
    {
        fprintf(stderr, "Audren: audrenStartAudioRenderer failed: 0x%08X\n", rc);
        return false;
    }

    m_initialized = true;
    printf("Audren: %d voices, %dHz, %dch\n", m_maxVoices, sampleRate, channels);
    return true;
}

void AudrenAudio::Update()
{
    if (m_initialized)
        audrenWaitFrame();
}

void AudrenAudio::Shutdown()
{
    if (m_initialized)
    {
        audrenStopAudioRenderer();
        audrenExit();
        delete[] m_waveBufs;
        m_waveBufs = nullptr;
        m_initialized = false;
    }
}