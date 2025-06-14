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
inline constexpr auto CONFIG_STRENGTH = "plugin:glasswindow:strength";
inline constexpr auto CONFIG_BLUR = "plugin:glasswindow:blur_radius";
inline constexpr auto CONFIG_CHROMATIC = "plugin:glasswindow:chromatic";
inline constexpr auto CONFIG_CHROMATIC_STRENGTH = "plugin:glasswindow:chromatic_strength";
inline constexpr auto CONFIG_BRIGHTNESS = "plugin:glasswindow:brightness";
inline constexpr auto CONFIG_CONTRAST = "plugin:glasswindow:contrast";
inline constexpr auto CONFIG_SATURATION = "plugin:glasswindow:saturation";
inline constexpr auto CONFIG_RULES = "plugin:glasswindow:rules";

// Plugin handle
inline HANDLE PHANDLE = nullptr;

struct GlassConfig {
    float strength = 1.0f;
    int blur_radius = 10;
    int chromatic = 0;
    float chromatic_strength = 0.5f;
    float brightness = 1.0f;
    float contrast = 1.0f;
    float saturation = 1.0f;
    std::vector<std::string> rules;
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

private:
    CGlassWindow() = default;
    ~CGlassWindow() = default;
    
    CGlassWindow(const CGlassWindow&) = delete;
    CGlassWindow& operator=(const CGlassWindow&) = delete;
    
    GlassConfig m_config;
    std::unordered_set<PHLWINDOW> m_glassWindows;
};