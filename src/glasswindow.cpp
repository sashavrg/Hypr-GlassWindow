#include "../include/glasswindow.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <string>
#include <any>
#include <functional>

extern HANDLE PHANDLE;

CGlassWindow::CGlassWindow() {
    Debug::log(LOG, "GlassWindow: Constructor called");
}

CGlassWindow::~CGlassWindow() {
    Debug::log(LOG, "GlassWindow: Destructor called");
}

void CGlassWindow::init() {
    Debug::log(LOG, "GlassWindow: Initializing plugin");
    
    // Register config values
    const std::string strengthKey = "plugin:glasswindow:strength";
    const std::string chromaticKey = "plugin:glasswindow:chromatic_aberration";
    
    Debug::log(LOG, "GlassWindow: Registering config values");
    Debug::log(LOG, "GlassWindow: strength key: " + strengthKey);
    Debug::log(LOG, "GlassWindow: chromatic key: " + chromaticKey);
    
    bool strengthRegistered = HyprlandAPI::addConfigValue(PHANDLE, strengthKey, Hyprlang::CConfigValue(Hyprlang::FLOAT(0.7f)));
    bool chromaticRegistered = HyprlandAPI::addConfigValue(PHANDLE, chromaticKey, Hyprlang::CConfigValue(Hyprlang::INT(1)));
    
    std::string msg = "GlassWindow: Config registration - strength: ";
    msg += strengthRegistered ? "success" : "failed";
    msg += ", chromatic: ";
    msg += chromaticRegistered ? "success" : "failed";
    Debug::log(LOG, msg);

    // Register window hooks
    const std::string renderEvent = "renderWindow";
    const std::string createEvent = "windowCreated";
    const std::string destroyEvent = "windowDestroyed";

    Debug::log(LOG, "GlassWindow: Registering window hooks");
    
    static auto PHOOK = HyprlandAPI::registerCallbackDynamic(PHANDLE, renderEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        Debug::log(LOG, "GlassWindow: Render callback triggered");
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            renderWindow(PWINDOW);
    });

    static auto PHOOK2 = HyprlandAPI::registerCallbackDynamic(PHANDLE, createEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        Debug::log(LOG, "GlassWindow: Window create callback triggered");
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            onWindowCreate(PWINDOW);
    });

    static auto PHOOK3 = HyprlandAPI::registerCallbackDynamic(PHANDLE, destroyEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        Debug::log(LOG, "GlassWindow: Window destroy callback triggered");
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            onWindowDestroy(PWINDOW);
    });

    Debug::log(LOG, "GlassWindow: Hooks registered");
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

    // Apply the glass effect through Hyprland's rendering pipeline
    // This is a placeholder for the actual effect implementation
    // You'll need to implement the actual OpenGL shader logic here
    Debug::log(LOG, "GlassWindow: Applying effect to window");
}

bool CGlassWindow::shouldApplyToWindow(CWindow* pWindow) {
    if (!pWindow)
        return false;

    // Apply to all windows for now
    // You can add more sophisticated rules here later
    return true;
}

// Plugin entry point
extern "C" {
    __attribute__((visibility("default"))) void* createInstance() {
        Debug::log(LOG, "GlassWindow: Plugin instance created");
        return new CGlassWindow();
    }
} 