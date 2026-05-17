#include <switch.h>
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "engine/Fallout3Game.h"

static EGLDisplay s_eglDisplay = EGL_NO_DISPLAY;
static EGLSurface s_eglSurface = EGL_NO_SURFACE;
static EGLContext s_eglContext = EGL_NO_CONTEXT;
static Fallout3Game* g_pGame = nullptr;

extern "C" void userAppInit(void)
{
    romfsInit();
    fsdevMountSdmc();
    appletSetCpuBoostMode(AppletCpuBoostMode_Deterministic);
}

extern "C" void userAppExit(void)
{
    fsdevUnmountAll();
    romfsExit();
}

static bool InitEGL(void)
{
    s_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (s_eglDisplay == EGL_NO_DISPLAY)
    {
        fprintf(stderr, "EGL: Failed to get default display\n");
        return false;
    }

    EGLint major = 0, minor = 0;
    if (!eglInitialize(s_eglDisplay, &major, &minor))
    {
        fprintf(stderr, "EGL: eglInitialize failed: 0x%x\n", eglGetError());
        return false;
    }
    printf("EGL %d.%d\n", major, minor);

    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      24,
        EGL_STENCIL_SIZE,    8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(s_eglDisplay, configAttribs, &config, 1, &numConfigs))
    {
        fprintf(stderr, "EGL: No suitable config: 0x%x\n", eglGetError());
        return false;
    }

    NWindow* win = nwindowGetDefault();
    if (!win)
    {
        fprintf(stderr, "EGL: nwindowGetDefault returned NULL\n");
        return false;
    }

    s_eglSurface = eglCreateWindowSurface(s_eglDisplay, config, (NativeWindowType)win, nullptr);
    if (s_eglSurface == EGL_NO_SURFACE)
    {
        fprintf(stderr, "EGL: eglCreateWindowSurface failed: 0x%x\n", eglGetError());
        return false;
    }

    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    s_eglContext = eglCreateContext(s_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (s_eglContext == EGL_NO_CONTEXT)
    {
        fprintf(stderr, "EGL: eglCreateContext failed: 0x%x\n", eglGetError());
        return false;
    }

    if (!eglMakeCurrent(s_eglDisplay, s_eglSurface, s_eglSurface, s_eglContext))
    {
        fprintf(stderr, "EGL: eglMakeCurrent failed: 0x%x\n", eglGetError());
        return false;
    }

    printf("EGL: OK\n");
    return true;
}

static void DeinitEGL(void)
{
    if (s_eglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(s_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (s_eglContext != EGL_NO_CONTEXT) eglDestroyContext(s_eglDisplay, s_eglContext);
        if (s_eglSurface != EGL_NO_SURFACE) eglDestroySurface(s_eglDisplay, s_eglSurface);
        eglTerminate(s_eglDisplay);
    }
}

int main(int argc, char* argv[])
{
    hidInitialize();
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    if (!InitEGL())
    {
        fprintf(stderr, "FATAL: Graphics init failed\n");
        return -1;
    }

    size_t totalMem = 0, usedMem = 0;
    svcGetInfo(&totalMem, InfoType_TotalMemorySize, CUR_PROCESS_HANDLE, 0);
    svcGetInfo(&usedMem,  InfoType_UsedMemorySize,  CUR_PROCESS_HANDLE, 0);
    printf("Memory: %zu MB total, %zu MB available\n",
           totalMem / 1024 / 1024, (totalMem - usedMem) / 1024 / 1024);

    g_pGame = new Fallout3Game();
    if (!g_pGame->Initialize())
    {
        fprintf(stderr, "FATAL: Game init failed\n");
        delete g_pGame;
        DeinitEGL();
        return -2;
    }

    const u64 perfFreq = armGetSystemTickFreq();
    u64 lastTick = armGetSystemTick();

    while (appletMainLoop())
    {
        u64 nowTick = armGetSystemTick();
        float dt = (float)(nowTick - lastTick) / (float)perfFreq;
        lastTick = nowTick;
        if (dt > 0.1f) dt = 0.1f;

        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        u64 kHeld = padGetButtons(&pad);

        if (kDown & HidNpadButton_Plus) break;

        g_pGame->ProcessInput(kDown, kHeld, dt);
        g_pGame->Update(dt);
        g_pGame->Render();

        eglSwapBuffers(s_eglDisplay, s_eglSurface);
    }

    printf("Shutting down...\n");
    g_pGame->Shutdown();
    delete g_pGame;

    DeinitEGL();
    hidExit();

    printf("Goodbye.\n");
    return 0;
}