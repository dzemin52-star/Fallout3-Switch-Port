#pragma once

#include <cstdio>
#include <GLES3/gl3.h>

class SwitchRendererGL
{
public:
    void Init(int w, int h)
    {
        m_width  = w;
        m_height = h;
        glViewport(0, 0, w, h);
        printf("Renderer: %dx%d\n", w, h);
    }

    void BeginFrame()
    {
        glClearColor(0.12f, 0.18f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderScene()
    {
        // Placeholder
    }

    void EndFrame()
    {
        // Swap is handled by eglSwapBuffers in main loop
    }

private:
    int m_width = 1280;
    int m_height = 720;
};