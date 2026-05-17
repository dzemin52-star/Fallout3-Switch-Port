#pragma once

#include <switch.h>

class SwitchRendererGL;
class PhysicsSystemBullet;
class AudrenAudio;

class Fallout3Game
{
public:
    Fallout3Game();
    ~Fallout3Game();

    bool Initialize();
    void ProcessInput(u64 keysDown, u64 keysHeld, float dt);
    void Update(float dt);
    void Render();
    void Shutdown();

private:
    SwitchRendererGL*   m_renderer;
    PhysicsSystemBullet* m_physics;
    AudrenAudio*         m_audio;
};