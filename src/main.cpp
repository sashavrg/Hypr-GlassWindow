#include <hyprland/src/plugins/PluginAPI.hpp>
#include "../include/glasswindow.hpp"

HANDLE PHANDLE = nullptr;

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[GlassWindow] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[GlassWindow] Version mismatch");
    }

    // Initialize the glass window plugin using singleton
    CGlassWindow::getInstance().init();

    return {"GlassWindow", "A glass shader window effect plugin for Hyprland", "sashavrg", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    CGlassWindow::getInstance().cleanup();
} 