#include "../include/glasswindow.hpp"
#include <algorithm>
#include <regex>

extern HANDLE PHANDLE;

CGlassWindow& CGlassWindow::getInstance() {
    static CGlassWindow instance;
    return instance;
}

void CGlassWindow::init() {
    registerConfig();
    reloadConfig();

    // Register callbacks for window rendering and config reload
    static auto P = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "render",
        [](void* self, SCallbackInfo& info, std::any data) {
            CGlassWindow::getInstance().onRender();
        }
    );

    static auto P2 = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "configReloaded",
        [](void* self, SCallbackInfo& info, std::any data) {
            CGlassWindow::getInstance().reloadConfig();
        }
    );

    HyprlandAPI::addNotification(PHANDLE, "[GlassWindow] Initialized successfully!",
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
}

void CGlassWindow::cleanup() {
    m_glassWindows.clear();
    HyprlandAPI::addNotification(PHANDLE, "[GlassWindow] Cleaned up!",
                                 CHyprColor{0.2, 0.5, 1.0, 1.0}, 3000);
}

void CGlassWindow::registerConfig() {
    // Register all configuration values with defaults matching README
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_STRENGTH,
                                SConfigValue{.floatValue = 0.5f});

    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_BLUR_STEP,
                                SConfigValue{.floatValue = 0.01f});

    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC,
                                SConfigValue{.intValue = 0});

    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH,
                                SConfigValue{.floatValue = 0.005f});

    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_ALPHA,
                                SConfigValue{.floatValue = 1.0f});

    // Rules is a custom type - we'll handle it as a string list
    HyprlandAPI::addConfigValue(PHANDLE, CONFIG_RULES,
                                SConfigValue{.strValue = ""});
}

void CGlassWindow::reloadConfig() {
    // Reload all configuration values
    static auto* const PSTRENGTH = (Hyprlang::FLOAT* const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_STRENGTH)->getDataStaticPtr();
    static auto* const PBLURSTEP = (Hyprlang::FLOAT* const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_BLUR_STEP)->getDataStaticPtr();
    static auto* const PCHROMATIC = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC)->getDataStaticPtr();
    static auto* const PCHROMATICSTRENGTH = (Hyprlang::FLOAT* const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_CHROMATIC_STRENGTH)->getDataStaticPtr();
    static auto* const PALPHA = (Hyprlang::FLOAT* const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_ALPHA)->getDataStaticPtr();
    static auto* const PRULES = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, CONFIG_RULES)->getDataStaticPtr();

    if (PSTRENGTH)
        m_config.strength = **PSTRENGTH;

    if (PBLURSTEP)
        m_config.blur_step = **PBLURSTEP;

    if (PCHROMATIC)
        m_config.chromatic = **PCHROMATIC;

    if (PCHROMATICSTRENGTH)
        m_config.chromatic_strength = **PCHROMATICSTRENGTH;

    if (PALPHA)
        m_config.alpha = **PALPHA;

    if (PRULES) {
        parseRules(*PRULES);
    }

    HyprlandAPI::addNotification(PHANDLE, "[GlassWindow] Config reloaded",
                                 CHyprColor{0.2, 0.8, 1.0, 1.0}, 2000);
}

void CGlassWindow::parseRules(const std::string& rulesStr) {
    m_config.rules.clear();

    if (rulesStr.empty()) {
        return;
    }

    // Parse semicolon-separated rules
    std::string current;
    for (char c : rulesStr) {
        if (c == ';') {
            if (!current.empty()) {
                m_config.rules.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        m_config.rules.push_back(current);
    }
}

bool CGlassWindow::shouldApplyToWindow(PHLWINDOW pWindow) {
    if (!pWindow || !validMapped(pWindow)) {
        return false;
    }

    // If no rules specified, don't apply to any window
    if (m_config.rules.empty()) {
        return false;
    }

    // Check if any rule matches the window class or title
    std::string windowClass = pWindow->m_szInitialClass;
    std::string windowTitle = pWindow->m_szInitialTitle;

    for (const auto& rule : m_config.rules) {
        try {
            std::regex ruleRegex(rule, std::regex::icase | std::regex::ECMAScript);

            if (std::regex_search(windowClass, ruleRegex) ||
                std::regex_search(windowTitle, ruleRegex)) {
                return true;
            }
        } catch (const std::regex_error& e) {
            // Invalid regex, skip this rule
            continue;
        }
    }

    return false;
}

void CGlassWindow::applyGlassEffect(PHLWINDOW pWindow, CRegion& damage) {
    if (!pWindow || !shouldApplyToWindow(pWindow)) {
        return;
    }

    // Track this window as having glass effect applied
    m_glassWindows.insert(pWindow);

    // TODO: Implement actual shader-based glass effect rendering
    // This would involve:
    // 1. Setting up OpenGL shader programs with blur and chromatic aberration
    // 2. Capturing the window's framebuffer
    // 3. Applying multi-pass blur with m_config.blur_step
    // 4. Applying chromatic aberration if m_config.chromatic is enabled
    // 5. Adjusting final output with m_config.alpha
    //
    // For now, this is a placeholder that marks windows for effect application
}

void CGlassWindow::onRender() {
    // This gets called on each render frame
    // Iterate through all windows and apply glass effect where applicable
    for (auto& window : g_pCompositor->m_vWindows) {
        if (!validMapped(window)) {
            continue;
        }

        if (shouldApplyToWindow(window)) {
            CRegion emptyDamage;
            applyGlassEffect(window, emptyDamage);
        } else {
            // Remove from tracked windows if rule no longer matches
            m_glassWindows.erase(window);
        }
    }
}

void CGlassWindow::setShaderUniforms(CRenderInstance* pRenderer) {
    // TODO: Set shader uniforms when actual shader implementation is added
    // This would set:
    // - uniform float u_strength (m_config.strength)
    // - uniform float u_blur_step (m_config.blur_step)
    // - uniform int u_chromatic (m_config.chromatic)
    // - uniform float u_chromatic_strength (m_config.chromatic_strength)
    // - uniform float u_alpha (m_config.alpha)
}
