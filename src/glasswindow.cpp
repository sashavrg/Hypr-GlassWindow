#include "../include/glasswindow.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <any>
extern HANDLE PHANDLE;
#include <pixman-1/pixman.h>

CGlassWindow::CGlassWindow() {
    // Constructor
}

CGlassWindow::~CGlassWindow() {
    // Destructor
}

void CGlassWindow::init() {
    // Initialize plugin
    // Debug::log(LOG, "GlassWindow: Initializing plugin");
}

void CGlassWindow::onWindowCreate(CWindow* pWindow) {
    if (!pWindow)
        return;

    if (shouldApplyToWindow(pWindow)) {
        // Debug::log(LOG, "GlassWindow: Applying effect to window {}", pWindow->m_szTitle); // m_szTitle may not exist
        // Debug::log(LOG, "GlassWindow: Applying effect to window");
        // TODO: Apply glass effect
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
    auto strengthAny = HyprlandAPI::getConfigValue(PHANDLE, "plugin:glasswindow:strength")->getValue();
    float strength = std::any_cast<float>(strengthAny);
    auto chromaticAny = HyprlandAPI::getConfigValue(PHANDLE, "plugin:glasswindow:chromatic_aberration")->getValue();
    int chromatic = std::any_cast<int64_t>(chromaticAny);

    // TODO: Use 'strength' and 'chromatic' in the glass effect rendering logic
    // Example placeholder:
    // if (chromatic) { /* apply chromatic aberration effect with given strength */ }
    // else { /* apply glass effect without chromatic aberration */ }
}

bool CGlassWindow::shouldApplyToWindow(CWindow* pWindow) {
    if (!pWindow)
        return false;

    // TODO: Implement window rule checking
    return false;
}

// Plugin entry point
extern "C" {
    __attribute__((visibility("default"))) void* createInstance() {
        return new CGlassWindow();
    }
} 