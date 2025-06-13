#pragma once

#include <string>
#include <vector>
#include <any>
#include <functional>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/Shader.hpp>
#include <hyprutils/math/Region.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include </usr/include/pixman-1/pixman.h>

struct ShaderHolder {
    SShader CM;
    SShader RGBA;
    SShader RGBX;
    SShader EXT;

    void Init();
    void Destroy();
};

class CGlassWindow {
public:
    CGlassWindow();
    ~CGlassWindow();

    void init();
    void onWindowCreate(CWindow* pWindow);
    void onWindowDestroy(CWindow* pWindow);
    void renderWindow(CWindow* pWindow);

private:
    bool shouldApplyToWindow(CWindow* pWindow);
    std::vector<std::string> m_windowRules;
    ShaderHolder m_Shaders;
    bool m_ShadersSwapped = false;
}; 