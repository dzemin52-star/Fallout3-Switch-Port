#pragma once

#include <cstdint>
#include <cstddef>

class SwitchMemoryPool
{
public:
    static SwitchMemoryPool& Instance();

    enum PoolType
    {
        POOL_ACTORS = 0,
        POOL_ITEMS,
        POOL_EFFECTS,
        POOL_SCRIPTS,
        POOL_STREAMING,
        POOL_GEOMETRY,
        POOL_TEXTURES,
        POOL_AUDIO,
        POOL_MISC,
        NUM_POOLS
    };

    bool Initialize();
    void* Allocate(size_t size, PoolType type);
    void Free(void* ptr, PoolType type);
    size_t GetUsedMemory(PoolType type) const;
    size_t GetTotalMemory(PoolType type) const;
    void ForceGarbageCollection();

private:
    SwitchMemoryPool() = default;
    ~SwitchMemoryPool() = default;
    SwitchMemoryPool(const SwitchMemoryPool&) = delete;
    SwitchMemoryPool& operator=(const SwitchMemoryPool&) = delete;

    struct MemoryBlock
    {
        uint8_t* memory = nullptr;
        size_t   totalSize = 0;
        size_t   usedSize  = 0;
    };

    MemoryBlock m_pools[NUM_POOLS];
    bool        m_initialized = false;
};