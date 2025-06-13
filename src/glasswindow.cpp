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
    // Constructor
}

CGlassWindow::~CGlassWindow() {
    // Destructor
}

void CGlassWindow::init() {
    // Register config values
    const std::string strengthKey = "plugin:glasswindow:strength";
    const std::string chromaticKey = "plugin:glasswindow:chromatic_aberration";
    HyprlandAPI::addConfigValue(PHANDLE, strengthKey, Hyprlang::CConfigValue(Hyprlang::FLOAT(0.7f)));
    HyprlandAPI::addConfigValue(PHANDLE, chromaticKey, Hyprlang::CConfigValue(Hyprlang::INT(1)));

    // Register window hooks
    const std::string renderEvent = "renderWindow";
    const std::string createEvent = "windowCreated";
    const std::string destroyEvent = "windowDestroyed";

    static auto PHOOK = HyprlandAPI::registerCallbackDynamic(PHANDLE, renderEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            renderWindow(PWINDOW);
    });

    static auto PHOOK2 = HyprlandAPI::registerCallbackDynamic(PHANDLE, createEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            onWindowCreate(PWINDOW);
    });

    static auto PHOOK3 = HyprlandAPI::registerCallbackDynamic(PHANDLE, destroyEvent, [this](void* self, SCallbackInfo& info, std::any data) {
        auto* const PWINDOW = std::any_cast<CWindow*>(data);
        if (PWINDOW)
            onWindowDestroy(PWINDOW);
    });
}

void CGlassWindow::onWindowCreate(CWindow* pWindow) {
    if (!pWindow)
        return;

    if (shouldApplyToWindow(pWindow)) {
        Debug::log(LOG, "GlassWindow: Applying effect to window");
    }
}

void CGlassWindow::onWindowDestroy(CWindow* pWindow) {
    if (!pWindow)
        return;

    // Clean up any resources associated with the window
}

void CGlassWindow::renderWindow(CWindow* pWindow) {
    if (!pWindow || !shouldApplyToWindow(pWindow))
        return;

    // Read config values
    const std::string strengthKey = "plugin:glasswindow:strength";
    const std::string chromaticKey = "plugin:glasswindow:chromatic_aberration";
    auto strengthAny = HyprlandAPI::getConfigValue(PHANDLE, strengthKey)->getValue();
    float strength = std::any_cast<float>(strengthAny);
    auto chromaticAny = HyprlandAPI::getConfigValue(PHANDLE, chromaticKey)->getValue();
    int chromatic = std::any_cast<int64_t>(chromaticAny);

    // Apply glass effect
    if (pWindow->m_isX11) {
        // X11 windows need special handling
        return;
    }

    // Get the window's surface
    if (!pWindow->m_wlSurface || !pWindow->m_wlSurface->exists())
        return;

    // Apply the glass effect through Hyprland's rendering pipeline
    // This is a placeholder for the actual effect implementation
    // You'll need to implement the actual OpenGL shader logic here
    Debug::log(LOG, "GlassWindow: Rendering window with glass effect");
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
        return new CGlassWindow();
    }
} 