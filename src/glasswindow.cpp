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
#include <format>

extern HANDLE PHANDLE;

const std::vector<const char*> ASSET_PATHS = {
#ifdef DATAROOTDIR
    DATAROOTDIR,
#endif
    "/usr/share",
    "/usr/local/share",
};

static void logShaderError(const GLuint& shader, bool program, bool silent = false) {
    GLint maxLength = 0;
    if (program)
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    else
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    if (program)
        glGetProgramInfoLog(shader, maxLength, nullptr, errorLog.data());
    else
        glGetShaderInfoLog(shader, maxLength, nullptr, errorLog.data());
    std::string errorStr(errorLog.begin(), errorLog.end());

    const auto FULLERROR = (program ? "Shader parser: Error linking program:" : "Shader parser: Error compiling shader: ") + errorStr;

    Debug::log(ERR, "Failed to link shader: {}", FULLERROR);

    if (!silent)
        g_pConfigManager->addParseError(FULLERROR);
}

// OpenGL helper functions
static GLuint compileShader(const GLuint& type, std::string src, bool dynamic, bool silent) {
    auto shader = glCreateShader(type);
    auto shaderSource = src.c_str();
    glShaderSource(shader, 1, (const GLchar**)&shaderSource, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if (dynamic) {
        if (ok == GL_FALSE) {
            logShaderError(shader, false, silent);
            return 0;
        }
    } else {
        if (ok != GL_TRUE)
            logShaderError(shader, false);
        RASSERT(ok != GL_FALSE, "compileShader() failed! GL_COMPILE_STATUS not OK!");
    }

    return shader;
}

static GLuint createProgram(const std::string& vert, const std::string& frag, bool dynamic, bool silent) {
    auto vertCompiled = compileShader(GL_VERTEX_SHADER, vert, dynamic, silent);
    if (dynamic) {
        if (vertCompiled == 0)
            return 0;
    } else
        RASSERT(vertCompiled, "Compiling shader failed. VERTEX nullptr! Shader source:\n\n{}", vert);

    auto fragCompiled = compileShader(GL_FRAGMENT_SHADER, frag, dynamic, silent);
    if (dynamic) {
        if (fragCompiled == 0)
            return 0;
    } else
        RASSERT(fragCompiled, "Compiling shader failed. FRAGMENT nullptr! Shader source:\n\n{}", frag);

    auto prog = glCreateProgram();
    glAttachShader(prog, vertCompiled);
    glAttachShader(prog, fragCompiled);
    glLinkProgram(prog);

    glDetachShader(prog, vertCompiled);
    glDetachShader(prog, fragCompiled);
    glDeleteShader(vertCompiled);
    glDeleteShader(fragCompiled);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (dynamic) {
        if (ok == GL_FALSE) {
            logShaderError(prog, true, silent);
            return 0;
        }
    } else {
        if (ok != GL_TRUE)
            logShaderError(prog, true);
        RASSERT(ok != GL_FALSE, "createProgram() failed! GL_LINK_STATUS not OK!");
    }

    return prog;
}

static std::string loadShader(const std::string& filename) {
    const auto home = Hyprutils::Path::getHome();
    if (home) {
        const auto src = NFsUtils::readFileAsString(home.value() + "/hypr/shaders/" + filename);
        if (src)
            return src.value();
    }
    for (auto& e : ASSET_PATHS) {
        const auto src = NFsUtils::readFileAsString(std::string{e} + "/hypr/shaders/" + filename);
        if (src)
            return src.value();
    }
    throw std::runtime_error(std::format("Couldn't load shader {}", filename));
}

void ShaderHolder::Init() {
    g_pHyprRenderer->makeEGLCurrent();

    // Get shader sources from files
    const auto TEXVERTSRC = g_pHyprOpenGL->m_shaders->TEXVERTSRC;
    const auto TEXVERTSRC300 = g_pHyprOpenGL->m_shaders->TEXVERTSRC300;
    const auto TEXFRAGSRCCM = loadShader("CM.frag");
    const auto TEXFRAGSRCRGBA = loadShader("rgba.frag");
    const auto TEXFRAGSRCRGBX = loadShader("rgbx.frag");
    const auto TEXFRAGSRCEXT = loadShader("ext.frag");

    // Create shader programs
    CM.program = createProgram(TEXVERTSRC300, TEXFRAGSRCCM, true, true);
    if (!CM.program) throw std::runtime_error("Failed to create Shader: CM.frag, check hyprland logs");
    CM.proj = glGetUniformLocation(CM.program, "proj");
    CM.tex = glGetUniformLocation(CM.program, "tex");
    CM.texType = glGetUniformLocation(CM.program, "texType");
    CM.alphaMatte = glGetUniformLocation(CM.program, "texMatte");
    CM.alpha = glGetUniformLocation(CM.program, "alpha");
    CM.texAttrib = glGetAttribLocation(CM.program, "texcoord");
    CM.matteTexAttrib = glGetAttribLocation(CM.program, "texcoordMatte");
    CM.posAttrib = glGetAttribLocation(CM.program, "pos");
    CM.discardOpaque = glGetUniformLocation(CM.program, "discardOpaque");
    CM.discardAlpha = glGetUniformLocation(CM.program, "discardAlpha");
    CM.discardAlphaValue = glGetUniformLocation(CM.program, "discardAlphaValue");
    CM.applyTint = glGetUniformLocation(CM.program, "applyTint");
    CM.tint = glGetUniformLocation(CM.program, "tint");
    CM.useAlphaMatte = glGetUniformLocation(CM.program, "useAlphaMatte");
    CM.topLeft = glGetUniformLocation(CM.program, "topLeft");
    CM.fullSize = glGetUniformLocation(CM.program, "fullSize");
    CM.radius = glGetUniformLocation(CM.program, "radius");
    CM.roundingPower = glGetUniformLocation(CM.program, "roundingPower");

    RGBA.program = createProgram(TEXVERTSRC, TEXFRAGSRCRGBA, true, true);
    if (!RGBA.program) throw std::runtime_error("Failed to create Shader: rgba.frag, check hyprland logs");
    RGBA.proj = glGetUniformLocation(RGBA.program, "proj");
    RGBA.tex = glGetUniformLocation(RGBA.program, "tex");
    RGBA.alphaMatte = glGetUniformLocation(RGBA.program, "texMatte");
    RGBA.alpha = glGetUniformLocation(RGBA.program, "alpha");
    RGBA.texAttrib = glGetAttribLocation(RGBA.program, "texcoord");
    RGBA.matteTexAttrib = glGetAttribLocation(RGBA.program, "texcoordMatte");
    RGBA.posAttrib = glGetAttribLocation(RGBA.program, "pos");
    RGBA.discardOpaque = glGetUniformLocation(RGBA.program, "discardOpaque");
    RGBA.discardAlpha = glGetUniformLocation(RGBA.program, "discardAlpha");
    RGBA.discardAlphaValue = glGetUniformLocation(RGBA.program, "discardAlphaValue");
    RGBA.applyTint = glGetUniformLocation(RGBA.program, "applyTint");
    RGBA.tint = glGetUniformLocation(RGBA.program, "tint");
    RGBA.useAlphaMatte = glGetUniformLocation(RGBA.program, "useAlphaMatte");
    RGBA.topLeft = glGetUniformLocation(RGBA.program, "topLeft");
    RGBA.fullSize = glGetUniformLocation(RGBA.program, "fullSize");
    RGBA.radius = glGetUniformLocation(RGBA.program, "radius");
    RGBA.roundingPower = glGetUniformLocation(RGBA.program, "roundingPower");

    RGBX.program = createProgram(TEXVERTSRC, TEXFRAGSRCRGBX, true, true);
    if (!RGBX.program) throw std::runtime_error("Failed to create Shader: rgbx.frag, check hyprland logs");
    RGBX.tex = glGetUniformLocation(RGBX.program, "tex");
    RGBX.proj = glGetUniformLocation(RGBX.program, "proj");
    RGBX.alpha = glGetUniformLocation(RGBX.program, "alpha");
    RGBX.texAttrib = glGetAttribLocation(RGBX.program, "texcoord");
    RGBX.posAttrib = glGetAttribLocation(RGBX.program, "pos");
    RGBX.discardOpaque = glGetUniformLocation(RGBX.program, "discardOpaque");
    RGBX.discardAlpha = glGetUniformLocation(RGBX.program, "discardAlpha");
    RGBX.discardAlphaValue = glGetUniformLocation(RGBX.program, "discardAlphaValue");
    RGBX.applyTint = glGetUniformLocation(RGBX.program, "applyTint");
    RGBX.tint = glGetUniformLocation(RGBX.program, "tint");
    RGBX.topLeft = glGetUniformLocation(RGBX.program, "topLeft");
    RGBX.fullSize = glGetUniformLocation(RGBX.program, "fullSize");
    RGBX.radius = glGetUniformLocation(RGBX.program, "radius");
    RGBX.roundingPower = glGetUniformLocation(RGBX.program, "roundingPower");

    EXT.program = createProgram(TEXVERTSRC, TEXFRAGSRCEXT, true, true);
    if (!EXT.program) throw std::runtime_error("Failed to create Shader: ext.frag, check hyprland logs");
    EXT.tex = glGetUniformLocation(EXT.program, "tex");
    EXT.proj = glGetUniformLocation(EXT.program, "proj");
    EXT.alpha = glGetUniformLocation(EXT.program, "alpha");
    EXT.posAttrib = glGetAttribLocation(EXT.program, "pos");
    EXT.texAttrib = glGetAttribLocation(EXT.program, "texcoord");
    EXT.discardOpaque = glGetUniformLocation(EXT.program, "discardOpaque");
    EXT.discardAlpha = glGetUniformLocation(EXT.program, "discardAlpha");
    EXT.discardAlphaValue = glGetUniformLocation(EXT.program, "discardAlphaValue");
    EXT.applyTint = glGetUniformLocation(EXT.program, "applyTint");
    EXT.tint = glGetUniformLocation(EXT.program, "tint");
    EXT.topLeft = glGetUniformLocation(EXT.program, "topLeft");
    EXT.fullSize = glGetUniformLocation(EXT.program, "fullSize");
    EXT.radius = glGetUniformLocation(EXT.program, "radius");
    EXT.roundingPower = glGetUniformLocation(EXT.program, "roundingPower");

    g_pHyprRenderer->unsetEGL();
}

void ShaderHolder::Destroy() {
    g_pHyprRenderer->makeEGLCurrent();

    CM.destroy();
    RGBA.destroy();
    RGBX.destroy();
    EXT.destroy();

    g_pHyprRenderer->unsetEGL();
}

CGlassWindow::CGlassWindow() {
    Debug::log(LOG, "GlassWindow: Constructor called");
}

CGlassWindow::~CGlassWindow() {
    Debug::log(LOG, "GlassWindow: Destructor called");
    if (m_ShadersSwapped) {
        std::swap(m_Shaders.CM, g_pHyprOpenGL->m_shaders->m_shCM);
        std::swap(m_Shaders.RGBA, g_pHyprOpenGL->m_shaders->m_shRGBA);
        std::swap(m_Shaders.RGBX, g_pHyprOpenGL->m_shaders->m_shRGBX);
        std::swap(m_Shaders.EXT, g_pHyprOpenGL->m_shaders->m_shEXT);
        m_ShadersSwapped = false;
    }
    m_Shaders.Destroy();
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

    // Initialize shaders
    m_Shaders.Init();

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