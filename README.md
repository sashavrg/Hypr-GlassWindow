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

- plugin:glasswindow:strength (float): Overall effect strength

- plugin:glasswindow:blur_radius (int): Blur radius for the glass effect

- plugin:glasswindow:chromatic (int): Enable chromatic aberration (0 or 1)

- plugin:glasswindow:chromatic_strength (float): Chromatic aberration strength

- plugin:glasswindow:brightness (float): Brightness adjustment

- plugin:glasswindow:contrast (float): Contrast adjustment

- plugin:glasswindow:saturation (float): Saturation adjustment

- plugin:glasswindow:rules (array of strings): Window class/title patterns to apply the effect

## development

this is my first C++ project, coming from just a limited GLSL background. all contributions are welceome, open issues, submit pull requests, do everything you like with this.

## license

MIT license © sashavrg