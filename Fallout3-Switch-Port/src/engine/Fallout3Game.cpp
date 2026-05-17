#include "Fallout3Game.h"
#include "../renderer/SwitchRendererGL.h"
#include "../physics/PhysicsBullet.h"
#include "../audio/AudrenAudio.h"
#include "../memory/SwitchMemoryPool.h"
#include "../debug/SwitchProfiler.h"
#include <cstdio>

Fallout3Game::Fallout3Game()
    : m_renderer(nullptr)
    , m_physics(nullptr)
    , m_audio(nullptr)
{}

Fallout3Game::~Fallout3Game()
{
    Shutdown();
}

bool Fallout3Game::Initialize()
{
    printf("Fallout3Game: Initializing...\n");

    SwitchMemoryPool::Instance().Initialize();

    m_renderer = new SwitchRendererGL();
    m_renderer->Init(1280, 720);

    m_physics = new PhysicsSystemBullet();
    m_physics->Init();

    m_audio = new AudrenAudio();
    if (!m_audio->Initialize(0x04000000, 48000, 2))
    {
        printf("Warning: Audio init failed, continuing without sound.\n");
    }

    printf("Fallout3Game: Ready.\n");
    return true;
}

void Fallout3Game::ProcessInput(u64 keysDown, u64 keysHeld, float dt)
{
    if (keysDown & HidNpadButton_B)
        printf("Player pressed B\n");
    if (keysDown & HidNpadButton_A)
        printf("Player pressed A\n");
}

void Fallout3Game::Update(float dt)
{
    SwitchProfiler::Instance().BeginFrame();

    m_physics->Update(dt);
    m_audio->Update();

    SwitchProfiler::Instance().EndFrame();
}

void Fallout3Game::Render()
{
    m_renderer->BeginFrame();
    m_renderer->RenderScene();
    m_renderer->EndFrame();
}

void Fallout3Game::Shutdown()
{
    printf("Fallout3Game: Shutting down...\n");
    delete m_renderer;
    delete m_physics;
    delete m_audio;
}