#include "../include/glasswindow.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/helpers/fs/FsUtils.hpp>
#include <hyprland/src/macros.hpp>
#include <string>
#include <any>
#include <functional>
#include <vector>
#include <hyprutils/string/String.hpp>
#include <hyprutils/path/Path.hpp>
#include <regex>

extern HANDLE PHANDLE;

// Plugin metadata
static const char* PLUGIN_NAME = "glasswindow";
static const char* PLUGIN_VERSION = "1.0.0";
static const char* PLUGIN_AUTHOR = "Your Name";
static const char* PLUGIN_DESCRIPTION = "Adds glass effect to windows";

// Configuration keys
static const char* CONFIG_STRENGTH = "plugin:glasswindow:strength";
static const char* CONFIG_BLUR = "plugin:glasswindow:blur_radius";
static const char* CONFIG_CHROMATIC = "plugin:glasswindow:chromatic";
static const char* CONFIG_CHROMATIC_STRENGTH = "plugin:glasswindow:chromatic_strength";
static const char* CONFIG_BRIGHTNESS = "plugin:glasswindow:brightness";
static const char* CONFIG_CONTRAST = "plugin:glasswindow:contrast";
static const char* CONFIG_SATURATION = "plugin:glasswindow:saturation";
static const char* CONFIG_RULES = "plugin:glasswindow:rules";

// SGlassConfig implementation
bool SGlassConfig::validate() const {
    if (strength < MIN_STRENGTH || strength > MAX_STRENGTH) return false;
    if (blur_radius < MIN_BLUR || blur_radius > MAX_BLUR) return false;
    if (chromatic_strength < MIN_CHROMATIC_STRENGTH || chromatic_strength > MAX_CHROMATIC_STRENGTH) return false;
    if (brightness < MIN_BRIGHTNESS || brightness > MAX_BRIGHTNESS) return false;
    if (contrast < MIN_CONTRAST || contrast > MAX_CONTRAST) return false;
    if (saturation < MIN_SATURATION || saturation > MAX_SATURATION) return false;
    return true;
}

void SGlassConfig::clampValues() {
    strength = std::clamp(strength, MIN_STRENGTH, MAX_STRENGTH);
    blur_radius = std::clamp(blur_radius, MIN_BLUR, MAX_BLUR);
    chromatic_strength = std::clamp(chromatic_strength, MIN_CHROMATIC_STRENGTH, MAX_CHROMATIC_STRENGTH);
    brightness = std::clamp(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    contrast = std::clamp(contrast, MIN_CONTRAST, MAX_CONTRAST);
    saturation = std::clamp(saturation, MIN_SATURATION, MAX_SATURATION);
}

// ShaderHolder implementation
void ShaderHolder::reloadShaders() {
    Destroy();
    Init();
}

bool ShaderHolder::validateShaders() const {
    return CM.program && RGBA.program && RGBX.program && EXT.program;
}

// CGlassWindow implementation
void CGlassWindow::registerConfig() {
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_STRENGTH, SConfigValue{.floatValue = m_config.strength});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_BLUR, SConfigValue{.intValue = m_config.blur_radius});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC, SConfigValue{.intValue = m_config.chromatic});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH, SConfigValue{.floatValue = m_config.chromatic_strength});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_BRIGHTNESS, SConfigValue{.floatValue = m_config.brightness});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CONTRAST, SConfigValue{.floatValue = m_config.contrast});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_SATURATION, SConfigValue{.floatValue = m_config.saturation});
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_RULES, SConfigValue{.arrayValue = m_config.rules});
}

void CGlassWindow::reloadConfig() {
    try {
        auto* config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_STRENGTH);
        if (config) m_config.strength = config->floatValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_BLUR);
        if (config) m_config.blur_radius = config->intValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC);
        if (config) m_config.chromatic = config->intValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH);
        if (config) m_config.chromatic_strength = config->floatValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_BRIGHTNESS);
        if (config) m_config.brightness = config->floatValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CONTRAST);
        if (config) m_config.contrast = config->floatValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_SATURATION);
        if (config) m_config.saturation = config->floatValue;
        
        config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_RULES);
        if (config) m_config.rules = config->arrayValue;

        m_config.clampValues();
        if (!m_config.validate()) {
            Debug::log(ERR, "GlassWindow: Invalid configuration values detected");
            return;
        }

        updateShaderUniforms();
    } catch (const std::exception& e) {
        Debug::log(ERR, "GlassWindow: Failed to reload configuration: " + std::string(e.what()));
    }
}

void CGlassWindow::updateShaderUniforms() {
    if (!m_Shaders.validateShaders()) {
        Debug::log(ERR, "GlassWindow: Invalid shaders, cannot update uniforms");
        return;
    }

    g_pHyprRenderer->makeEGLCurrent();

    // Update uniforms for all shaders
    for (auto* shader : {&m_Shaders.CM, &m_Shaders.RGBA, &m_Shaders.RGBX, &m_Shaders.EXT}) {
        glUseProgram(shader->program);
        glUniform1f(glGetUniformLocation(shader->program, "u_strength"), m_config.strength);
        glUniform1i(glGetUniformLocation(shader->program, "u_blur_radius"), m_config.blur_radius);
        glUniform1i(glGetUniformLocation(shader->program, "u_chromatic"), m_config.chromatic);
        glUniform1f(glGetUniformLocation(shader->program, "u_chromatic_strength"), m_config.chromatic_strength);
        glUniform1f(glGetUniformLocation(shader->program, "u_brightness"), m_config.brightness);
        glUniform1f(glGetUniformLocation(shader->program, "u_contrast"), m_config.contrast);
        glUniform1f(glGetUniformLocation(shader->program, "u_saturation"), m_config.saturation);
    }

    g_pHyprRenderer->unsetEGL();
}

bool CGlassWindow::shouldApplyToWindow(CWindow* pWindow) {
    if (!pWindow || !pWindow->m_bMapped) return false;

    // Check window rules
    std::vector<SP<CWindowRule>> rules = g_pConfigManager->getMatchingRules(pWindow);
    bool shouldApply = std::any_of(rules.begin(), rules.end(), [](const SP<CWindowRule>& rule) {
        return rule->m_rule == "plugin:glasswindow";
    });

    if (shouldApply) return true;

    // Check custom rules
    const auto& className = pWindow->m_szClass;
    const auto& title = pWindow->m_szTitle;

    for (const auto& rule : m_config.rules) {
        try {
            if (rule.starts_with("class:")) {
                std::regex pattern(rule.substr(6));
                if (std::regex_match(className, pattern)) return true;
            } else if (rule.starts_with("title:")) {
                std::regex pattern(rule.substr(6));
                if (std::regex_match(title, pattern)) return true;
            } else if (rule.starts_with("!class:")) {
                std::regex pattern(rule.substr(7));
                if (std::regex_match(className, pattern)) return false;
            } else if (rule.starts_with("!title:")) {
                std::regex pattern(rule.substr(7));
                if (std::regex_match(title, pattern)) return false;
            }
        } catch (const std::regex_error& e) {
            Debug::log(ERR, "GlassWindow: Invalid regex in rule: " + rule);
        }
    }

    return true;
}

void CGlassWindow::init() {
    Debug::log(LOG, "GlassWindow: Initializing plugin");
    
    // Check Hyprland version
    const auto PLUGIN_HYPRLAND_VERSION = HyprlandAPI::getHyprlandVersion(PHANDLE);
    if (!PLUGIN_HYPRLAND_VERSION) {
        Debug::log(ERR, "GlassWindow: Failed to get Hyprland version");
        return;
    }

    // Register configuration
    registerConfig();
    reloadConfig();

    // Initialize shaders
    try {
        m_Shaders.Init();
        if (!m_Shaders.validateShaders()) {
            Debug::log(ERR, "GlassWindow: Failed to initialize shaders");
            return;
        }
    } catch (const std::exception& e) {
        Debug::log(ERR, "GlassWindow: Failed to initialize shaders: " + std::string(e.what()));
        return;
    }

    // Register callbacks
    static auto PHOOK = HyprlandAPI::registerCallbackDynamic(PHANDLE, "renderWindow", [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (!PWINDOW || !shouldApplyToWindow(PWINDOW)) return;

        if (!m_ShadersSwapped) {
            std::swap(m_Shaders.CM, g_pHyprOpenGL->m_shaders->m_shCM);
            std::swap(m_Shaders.RGBA, g_pHyprOpenGL->m_shaders->m_shRGBA);
            std::swap(m_Shaders.RGBX, g_pHyprOpenGL->m_shaders->m_shRGBX);
            std::swap(m_Shaders.EXT, g_pHyprOpenGL->m_shaders->m_shEXT);
            m_ShadersSwapped = true;
        }
    });

    static auto PHOOK2 = HyprlandAPI::registerCallbackDynamic(PHANDLE, "windowCreated", [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW) onWindowCreate(PWINDOW);
    });

    static auto PHOOK3 = HyprlandAPI::registerCallbackDynamic(PHANDLE, "windowDestroyed", [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW) onWindowDestroy(PWINDOW);
    });

    static auto PHOOK4 = HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [this](void* self, SCallbackInfo& info, std::any data) {
        reloadConfig();
    });

    m_initialized = true;
    Debug::log(LOG, "GlassWindow: Plugin initialized successfully");
}

void CGlassWindow::onWindowCreate(CWindow* pWindow) {
    if (!pWindow)
        return;

    Debug::log(LOG, "GlassWindow: Window created");
    
    if (shouldApplyToWindow(pWindow)) {
        Debug::log(LOG, "GlassWindow: Applying effect to window");
    }
}

void CGlassWindow::onWindowDestroy(CWindow* pWindow) {
    if (!pWindow)
        return;

    Debug::log(LOG, "GlassWindow: Window destroyed");
}

void CGlassWindow::renderWindow(CWindow* pWindow) {
    if (!pWindow || !shouldApplyToWindow(pWindow))
        return;

    // Read config values
    const std::string strengthKey = "plugin:glasswindow:strength";
    const std::string chromaticKey = "plugin:glasswindow:chromatic_aberration";
    
    Debug::log(LOG, "GlassWindow: Reading config values");
    Debug::log(LOG, "GlassWindow: strength key: " + strengthKey);
    Debug::log(LOG, "GlassWindow: chromatic key: " + chromaticKey);
    
    auto strengthValue = HyprlandAPI::getConfigValue(PHANDLE, strengthKey);
    auto chromaticValue = HyprlandAPI::getConfigValue(PHANDLE, chromaticKey);
    
    if (!strengthValue || !chromaticValue) {
        Debug::log(ERR, "GlassWindow: Failed to get config values");
        return;
    }

    auto strengthAny = strengthValue->getValue();
    auto chromaticAny = chromaticValue->getValue();
    
    float strength = std::any_cast<float>(strengthAny);
    int chromatic = std::any_cast<int64_t>(chromaticAny);
    
    std::string msg = "GlassWindow: Rendering window with strength: ";
    msg += std::to_string(strength);
    msg += ", chromatic: ";
    msg += std::to_string(chromatic);
    Debug::log(LOG, msg);

    // Apply glass effect
    if (pWindow->m_isX11) {
        Debug::log(LOG, "GlassWindow: Skipping X11 window");
        return;
    }

    // Get the window's surface
    if (!pWindow->m_wlSurface || !pWindow->m_wlSurface->exists()) {
        Debug::log(LOG, "GlassWindow: No valid surface for window");
        return;
    }

    // Apply the glass effect by swapping shaders
    std::swap(m_Shaders.CM, g_pHyprOpenGL->m_shaders->m_shCM);
    std::swap(m_Shaders.RGBA, g_pHyprOpenGL->m_shaders->m_shRGBA);
    std::swap(m_Shaders.RGBX, g_pHyprOpenGL->m_shaders->m_shRGBX);
    std::swap(m_Shaders.EXT, g_pHyprOpenGL->m_shaders->m_shEXT);
    m_ShadersSwapped = true;

    // Restore original shaders after rendering
    std::swap(m_Shaders.CM, g_pHyprOpenGL->m_shaders->m_shCM);
    std::swap(m_Shaders.RGBA, g_pHyprOpenGL->m_shaders->m_shRGBA);
    std::swap(m_Shaders.RGBX, g_pHyprOpenGL->m_shaders->m_shRGBX);
    std::swap(m_Shaders.EXT, g_pHyprOpenGL->m_shaders->m_shEXT);
    m_ShadersSwapped = false;
}

// Plugin entry point
extern "C" {
    __attribute__((visibility("default"))) void* createInstance() {
        Debug::log(LOG, "GlassWindow: Plugin instance created");
        return new CGlassWindow();
    }
} 