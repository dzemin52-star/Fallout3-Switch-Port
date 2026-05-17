#include "SwitchMemoryPool.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

SwitchMemoryPool& SwitchMemoryPool::Instance()
{
    static SwitchMemoryPool inst;
    return inst;
}

bool SwitchMemoryPool::Initialize()
{
    if (m_initialized)
        return true;

    const size_t poolSizes[NUM_POOLS] = {
        0x01800000, // ACTORS    24 MB
        0x01000000, // ITEMS     16 MB
        0x00800000, // EFFECTS    8 MB
        0x01000000, // SCRIPTS   16 MB
        0x04000000, // STREAMING 64 MB
        0x02000000, // GEOMETRY  32 MB
        0x03000000, // TEXTURES  48 MB
        0x02000000, // AUDIO     32 MB
        0x02000000  // MISC      32 MB
    };

    const char* poolNames[NUM_POOLS] = {
        "Actors", "Items", "Effects", "Scripts", "Streaming",
        "Geometry", "Textures", "Audio", "Misc"
    };

    for (int i = 0; i < NUM_POOLS; ++i)
    {
        m_pools[i].memory = (uint8_t*)memalign(0x1000, poolSizes[i]);
        if (!m_pools[i].memory)
        {
            fprintf(stderr, "Memory: FAILED to allocate %s pool (%zu MB)\n",
                    poolNames[i], poolSizes[i] / 1024 / 1024);
            return false;
        }
        m_pools[i].totalSize = poolSizes[i];
        m_pools[i].usedSize  = 0;
        printf("Memory: %s pool = %zu MB\n", poolNames[i], poolSizes[i] / 1024 / 1024);
    }

    m_initialized = true;
    printf("Memory: All pools initialized\n");
    return true;
}

void* SwitchMemoryPool::Allocate(size_t size, PoolType type)
{
    if (type >= NUM_POOLS || !m_pools[type].memory)
        return nullptr;

    if (m_pools[type].usedSize + size > m_pools[type].totalSize)
    {
        fprintf(stderr, "Memory: Out of memory in pool %d, forcing GC...\n", (int)type);
        ForceGarbageCollection();
        if (m_pools[type].usedSize + size > m_pools[type].totalSize)
            return nullptr;
    }

    void* ptr = m_pools[type].memory + m_pools[type].usedSize;
    m_pools[type].usedSize += size;
    return ptr;
}

void SwitchMemoryPool::Free(void* ptr, PoolType type)
{
    (void)ptr;
    (void)type;
}

size_t SwitchMemoryPool::GetUsedMemory(PoolType type) const
{
    return (type < NUM_POOLS) ? m_pools[type].usedSize : 0;
}

size_t SwitchMemoryPool::GetTotalMemory(PoolType type) const
{
    return (type < NUM_POOLS) ? m_pools[type].totalSize : 0;
}

void SwitchMemoryPool::ForceGarbageCollection()
{
    printf("Memory: ForceGarbageCollection called (stub)\n");
}
