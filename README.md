# glasswindow — glass effect plugin for hyprland

**glasswindow** adds a customizable glass (blur + chromatic aberration) effect to windows in Hyprland, enhancing your compositor’s visuals with smooth refraction-like distortion and blur.

---

## features

- Configurable blur strength and radius  
- Optional chromatic aberration with adjustable strength  
- Brightness, contrast, and saturation tweaks  
- Rule-based application to specific windows by class or title patterns  
- Dynamic config reload without restarting Hyprland  
- Lightweight and performant  

---

## installation via hyperpm

1. ensure your hyprpm is updated:  
   ```bash
   hyprpm update
   ```
2. add the plugin via hyprpm
   ```bash
   hyprpm add https://github.com/sashavrg/Hypr-GlassWindow
   ```
3. enable the plugin:
   ```bash
   hyprpm enable glasswindow
   ```
4. reload hyprpm
   ```bash
   hyprpm reload
   ```

## configuration

Add these settings to your `~/.config/hypr/hyprland.conf`:

- **plugin:glasswindow:strength** (float, default: 0.5): Overall blur effect strength (0.0 - 1.0)

- **plugin:glasswindow:blur_step** (float, default: 0.01): UV step size for blur sampling

- **plugin:glasswindow:chromatic** (int, default: 0): Enable chromatic aberration (0 = disabled, 1 = enabled)

- **plugin:glasswindow:chromatic_strength** (float, default: 0.005): Chromatic aberration offset strength

- **plugin:glasswindow:alpha** (float, default: 1.0): Overall transparency (0.0 = fully transparent, 1.0 = opaque)

- **plugin:glasswindow:rules** (string): Semicolon-separated regex patterns for window class/title matching

### example configuration

```conf
plugin {
    glasswindow {
        strength = 0.7
        blur_step = 0.01
        chromatic = 1
        chromatic_strength = 0.008
        alpha = 0.95
        rules = ^(kitty|alacritty)$;^Mozilla Firefox$
    }
}
```

## development

this is my first C++ project, coming from just a limited GLSL background. all contributions are welceome, open issues, submit pull requests, do everything you like with this.

## license

MIT license © sashavrg