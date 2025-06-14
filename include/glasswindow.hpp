#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprlang.hpp>

#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

// Configuration keys
constexpr const char* CONFIG_STRENGTH = "plugin:glasswindow:strength";
constexpr const char* CONFIG_BLUR_STEP = "plugin:glasswindow:blur_step";
constexpr const char* CONFIG_CHROMATIC = "plugin:glasswindow:chromatic";
constexpr const char* CONFIG_CHROMATIC_STRENGTH = "plugin:glasswindow:chromatic_strength";
constexpr const char* CONFIG_ALPHA = "plugin:glasswindow:alpha";
constexpr const char* CONFIG_RULES = "plugin:glasswindow:rules";

// Plugin handle
inline HANDLE PHANDLE = nullptr;

struct GlassConfig {
    float strength = 0.5f;             // blur amount 0-1
    float blur_step = 0.01f;           // UV step size for blur
    int chromatic = 0;                 // chromatic aberration enabled
    float chromatic_strength = 0.005f; // chromatic aberration offset
    float alpha = 1.0f;                // overall transparency
    std::vector<std::string> rules;   // rules to decide where to apply effect
};

class CGlassWindow {
public:
    static CGlassWindow& getInstance();

    void init();
    void cleanup();
    void registerConfig();
    void reloadConfig();

    bool shouldApplyToWindow(PHLWINDOW pWindow);
    void applyGlassEffect(PHLWINDOW pWindow, CRegion& damage);

    // Helper for shader uniform setting
    void setShaderUniforms(CRenderInstance* pRenderer);

private:
    CGlassWindow() = default;
    ~CGlassWindow() = default;

    CGlassWindow(const CGlassWindow&) = delete;
    CGlassWindow& operator=(const CGlassWindow&) = delete;

    GlassConfig m_config;
    std::unordered_set<PHLWINDOW> m_glassWindows;
};
