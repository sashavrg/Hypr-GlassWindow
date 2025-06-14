#version 300 es
precision highp float;

uniform sampler2D tex;
uniform float alpha;
uniform float strength;        // overall glass strength (0-1)
uniform float blur_step;       // UV step size for blur sampling
uniform int chromatic;         // 0 = off, 1 = on
uniform float chromatic_strength; // chromatic aberration offset
uniform vec2 resolution;       // viewport size (for distortion scale)
in vec2 v_texcoord;
out vec4 fragColor;

// Simple pseudo-random noise based on UV coords
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

// 2D noise for distortion offset
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0 - 2.0*f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    // Calculate distortion offset
    float distortion_strength = strength * 0.02;
    vec2 distortion = vec2(
        noise(v_texcoord * resolution.xy * 0.5),
        noise(v_texcoord * resolution.xy * 0.5 + 100.0)
    ) * distortion_strength;

    // Accumulate blur color with offset distortion
    vec4 blurColor = vec4(0.0);
    float total = 0.0;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec2 offset = vec2(float(x), float(y)) * blur_step + distortion;
            blurColor += texture(tex, v_texcoord + offset);
            total += 1.0;
        }
    }
    blurColor /= total;

    // Original color
    vec4 origColor = texture(tex, v_texcoord);

    // Mix original and blurred with strength
    vec4 color = mix(origColor, blurColor, strength);

    // Chromatic aberration
    if (chromatic == 1) {
        vec2 chromaOffset = vec2(chromatic_strength);
        vec4 r = texture(tex, v_texcoord + chromaOffset);
        vec4 b = texture(tex, v_texcoord - chromaOffset);
        color.r = r.r;
        color.b = b.b;
    }

    // Apply alpha
    color.a *= alpha;

    fragColor = color;
}
