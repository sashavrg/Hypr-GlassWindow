#include "glasswindow.hpp"
#include <Compositor.hpp>
#include <desktop/Window.hpp>
#include <config/ConfigManager.hpp>
#include <plugins/PluginAPI.hpp>

// Plugin metadata
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[glasswindow] Mismatched headers! Can't proceed.", CColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[glasswindow] Version mismatch");
    }

    auto* const PPLUGINFUNC = HyprlandAPI::getFunctionByName(PHANDLE, "glasswindow");
    if (PPLUGINFUNC) {
        HyprlandAPI::addNotification(PHANDLE, "[glasswindow] Glass window plugin initialized!", CColor{0.2, 1.0, 0.2, 1.0}, 5000);
    }

    // Initialize the glass window system
    CGlassWindow::getInstance().init();

    return {"glasswindow", "Adds glass effect to windows", "Your Name", "1.0.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    CGlassWindow::getInstance().cleanup();
}

// Singleton instance
CGlassWindow& CGlassWindow::getInstance() {
    static CGlassWindow instance;
    return instance;
}

void CGlassWindow::registerConfig() {
    // Register configuration values using the modern API
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_STRENGTH, 
        Hyprlang::CConfigValue((float)m_config.strength));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_BLUR, 
        Hyprlang::CConfigValue((int64_t)m_config.blur_radius));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC, 
        Hyprlang::CConfigValue((int64_t)m_config.chromatic));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH, 
        Hyprlang::CConfigValue((float)m_config.chromatic_strength));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_BRIGHTNESS, 
        Hyprlang::CConfigValue((float)m_config.brightness));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CONTRAST, 
        Hyprlang::CConfigValue((float)m_config.contrast));
    
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_SATURATION, 
        Hyprlang::CConfigValue((float)m_config.saturation));
    
    // For array values, we need to handle them differently
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_RULES, 
        Hyprlang::CConfigValue(std::vector<std::string>{}));
}

void CGlassWindow::reloadConfig() {
    // Reload configuration values using the modern API
    auto* config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_STRENGTH);
    if (config) {
        try {
            m_config.strength = std::any_cast<float>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            // Fallback or error handling
            m_config.strength = 1.0f;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_BLUR);
    if (config) {
        try {
            m_config.blur_radius = std::any_cast<int64_t>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.blur_radius = 10;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC);
    if (config) {
        try {
            m_config.chromatic = std::any_cast<int64_t>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.chromatic = 0;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH);
    if (config) {
        try {
            m_config.chromatic_strength = std::any_cast<float>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.chromatic_strength = 0.5f;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_BRIGHTNESS);
    if (config) {
        try {
            m_config.brightness = std::any_cast<float>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.brightness = 1.0f;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CONTRAST);
    if (config) {
        try {
            m_config.contrast = std::any_cast<float>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.contrast = 1.0f;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_SATURATION);
    if (config) {
        try {
            m_config.saturation = std::any_cast<float>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.saturation = 1.0f;
        }
    }

    config = HyprlandAPI::getConfigValue(PHANDLE, CONFIG_RULES);
    if (config) {
        try {
            m_config.rules = std::any_cast<std::vector<std::string>>(config->getValue());
        } catch (const std::bad_any_cast& e) {
            m_config.rules.clear();
        }
    }
}

bool CGlassWindow::shouldApplyToWindow(PHLWINDOW pWindow) {
    if (!pWindow || !pWindow->m_isMapped) return false;

    // Get matching rules using the modern API
    std::vector<SP<CWindowRule>> rules = g_pConfigManager->getMatchingRules(pWindow);
    
    // Check if any rule matches our glass effect rules
    for (const auto& rule : rules) {
        if (rule->szRule.starts_with("glass")) {
            return true;
        }
    }

    // Check against our custom rules
    const auto& className = pWindow->m_class;
    const auto& title = pWindow->m_title;
    
    for (const auto& rule : m_config.rules) {
        // Simple pattern matching - you might want to enhance this
        if (className.find(rule) != std::string::npos || 
            title.find(rule) != std::string::npos) {
            return true;
        }
    }

    return false;
}

void CGlassWindow::applyGlassEffect(PHLWINDOW pWindow, CRegion& damage) {
    if (!shouldApplyToWindow(pWindow)) return;
    
    // Your glass effect implementation here
    // This is where you'd add the actual rendering logic
    
    // Example: Add to damage region to ensure proper rendering
    auto windowBox = pWindow->getWindowMainSurfaceBox();
    damage.add(windowBox);
}

void CGlassWindow::init() {
    // Check version compatibility
    if (PLUGIN_HYPRLAND_VERSION.hash.empty()) {
        HyprlandAPI::addNotification(PHANDLE, "[glasswindow] Version check failed!", 
                                   CColor{1.0, 0.2, 0.2, 1.0}, 5000);
        return;
    }

    registerConfig();
    reloadConfig();

    // Register callbacks using modern API
    static auto PHOOK = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "renderWindow", 
        [this](void*, SCallbackInfo&, std::any data) {
            auto pWindow = std::any_cast<PHLWINDOW>(data);
            if (shouldApplyToWindow(pWindow)) {
                // Apply glass effect rendering
                CRegion damage;
                applyGlassEffect(pWindow, damage);
            }
        });

    static auto PHOOK2 = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "windowCreated", 
        [this](void*, SCallbackInfo&, std::any data) {
            auto pWindow = std::any_cast<PHLWINDOW>(data);
            if (shouldApplyToWindow(pWindow)) {
                // Initialize glass effect for new window
                m_glassWindows.insert(pWindow);
            }
        });

    static auto PHOOK3 = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "windowDestroyed", 
        [this](void*, SCallbackInfo&, std::any data) {
            auto pWindow = std::any_cast<PHLWINDOW>(data);
            m_glassWindows.erase(pWindow);
        });

    static auto PHOOK4 = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "configReloaded", 
        [this](void*, SCallbackInfo&, std::any) {
            reloadConfig();
        });

    HyprlandAPI::addNotification(PHANDLE, "[glasswindow] Initialized successfully!", 
                               CColor{0.2, 1.0, 0.2, 1.0}, 5000);
}

void CGlassWindow::cleanup() {
    m_glassWindows.clear();
    HyprlandAPI::addNotification(PHANDLE, "[glasswindow] Cleaned up!", 
                               CColor{1.0, 1.0, 0.2, 1.0}, 3000);
}