# Hyprland Glass Window Effect

A Hyprland plugin that adds a beautiful glass effect to windows, with configurable strength, blur, and other visual parameters.

## Features

- Configurable glass effect strength
- Adjustable blur radius
- Chromatic aberration control
- Window-specific rules
- Real-time effect updates
- Performance optimized

## Requirements

- Hyprland (>= v0.33.1)
- OpenGL 3.3+
- CMake 3.16+
- C++23 compatible compiler

## Installation

### Using hyprpm (Recommended)

```bash
hyprpm update
hyprpm add https://github.com/yourusername/hyprland-glass-shader
hyprpm enable glasswindow
```

### Manual Installation

```bash
git clone https://github.com/yourusername/hyprland-glass-shader
cd hyprland-glass-shader/Hypr-GlassWindow
mkdir build && cd build
cmake ..
make
sudo make install
```

## Configuration

Add the following to your `hyprland.conf`:

```ini
# Load the plugin
exec-once = hyprctl plugin load /usr/local/lib/hyprland/plugins/libglasswindow.so

# Plugin configuration
plugin:glasswindow:strength = 0.5        # Glass effect strength (0.0 - 1.0)
plugin:glasswindow:blur_radius = 10      # Blur radius in pixels (0 - 100)
plugin:glasswindow:chromatic = 1         # Enable chromatic aberration (0 or 1)
plugin:glasswindow:chromatic_strength = 0.5  # Chromatic aberration strength (0.0 - 1.0)
plugin:glasswindow:brightness = 1.2      # Brightness multiplier (0.0 - 2.0)
plugin:glasswindow:contrast = 1.1        # Contrast multiplier (0.0 - 2.0)
plugin:glasswindow:saturation = 1.1      # Saturation multiplier (0.0 - 2.0)

# Window rules
plugin:glasswindow:rules = [
    "class:^(kitty)$",    # Apply to kitty terminal
    "class:^(firefox)$",  # Apply to Firefox
    "!class:^(discord)$"  # Don't apply to Discord
]
```

### Configuration Options

- `strength`: Controls the overall glass effect strength (0.0 - 1.0)
- `blur_radius`: Controls the blur amount in pixels (0 - 100)
- `chromatic`: Enables/disables chromatic aberration (0 or 1)
- `chromatic_strength`: Controls the chromatic aberration intensity (0.0 - 1.0)
- `brightness`: Adjusts the brightness of the effect (0.0 - 2.0)
- `contrast`: Adjusts the contrast of the effect (0.0 - 2.0)
- `saturation`: Adjusts the saturation of the effect (0.0 - 2.0)
- `rules`: List of window rules to control which windows get the effect

### Window Rules

Window rules use the following format:
- `class:^(regex)$` - Match window class
- `title:^(regex)$` - Match window title
- `!class:^(regex)$` - Exclude window class
- `!title:^(regex)$` - Exclude window title

## Troubleshooting

### Common Issues

1. **Plugin not loading**
   - Check if Hyprland version is >= 0.33.1
   - Verify plugin is installed in the correct location
   - Check Hyprland logs for errors

2. **No effect visible**
   - Verify configuration values are within valid ranges
   - Check if window rules are correctly configured
   - Ensure OpenGL 3.3+ is available

3. **Performance issues**
   - Reduce blur_radius
   - Disable chromatic aberration
   - Adjust window rules to apply to fewer windows

### Logs

Check Hyprland logs for detailed error messages:
```bash
hyprctl logs
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details. 