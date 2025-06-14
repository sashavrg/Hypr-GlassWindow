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

// Configuration structure
struct SGlassConfig {
    float strength = 0.5f;
    int blur_radius = 10;
    bool chromatic = true;
    float chromatic_strength = 0.5f;
    float brightness = 1.2f;
    float contrast = 1.1f;
    float saturation = 1.1f;
    std::vector<std::string> rules;

    // Validation ranges
    static constexpr float MIN_STRENGTH = 0.0f;
    static constexpr float MAX_STRENGTH = 1.0f;
    static constexpr int MIN_BLUR = 0;
    static constexpr int MAX_BLUR = 100;
    static constexpr float MIN_CHROMATIC_STRENGTH = 0.0f;
    static constexpr float MAX_CHROMATIC_STRENGTH = 1.0f;
    static constexpr float MIN_BRIGHTNESS = 0.0f;
    static constexpr float MAX_BRIGHTNESS = 2.0f;
    static constexpr float MIN_CONTRAST = 0.0f;
    static constexpr float MAX_CONTRAST = 2.0f;
    static constexpr float MIN_SATURATION = 0.0f;
    static constexpr float MAX_SATURATION = 2.0f;

    // Validation functions
    bool validate() const;
    void clampValues();
};

struct ShaderHolder {
    SShader CM;
    SShader RGBA;
    SShader RGBX;
    SShader EXT;

    void Init();
    void Destroy();
    void reloadShaders();
    bool validateShaders() const;
};

class CGlassWindow {
public:
    CGlassWindow();
    ~CGlassWindow();

    void init();
    void onWindowCreate(CWindow* pWindow);
    void onWindowDestroy(CWindow* pWindow);
    void renderWindow(CWindow* pWindow);
    void reloadConfig();

private:
    bool shouldApplyToWindow(CWindow* pWindow);
    bool validateConfig();
    void registerConfig();
    void updateShaderUniforms();

    std::vector<std::string> m_windowRules;
    ShaderHolder m_Shaders;
    bool m_ShadersSwapped = false;
    SGlassConfig m_config;
    bool m_initialized = false;
}; 