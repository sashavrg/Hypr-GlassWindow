#version 300 es
precision highp float;

uniform sampler2D tex;
uniform float alpha;
uniform vec2 topLeft;
uniform vec2 fullSize;
uniform float radius;
uniform float strength;
uniform int chromatic;

in vec2 v_texcoord;
out vec4 fragColor;

void main() {
    // Get the base color
    vec4 color = texture(tex, v_texcoord);
    
    // Apply glass effect
    float blur = strength * 0.1;
    vec4 blurColor = vec4(0.0);
    float total = 0.0;
    
    // Simple box blur
    for(float x = -2.0; x <= 2.0; x++) {
        for(float y = -2.0; y <= 2.0; y++) {
            vec2 offset = vec2(x, y) * blur;
            blurColor += texture(tex, v_texcoord + offset);
            total += 1.0;
        }
    }
    blurColor /= total;
    
    // Mix original color with blurred color
    color = mix(color, blurColor, strength);
    
    // Apply chromatic aberration if enabled
    if (chromatic > 0) {
        vec2 offset = vec2(strength * 0.01);
        vec4 r = texture(tex, v_texcoord + offset);
        vec4 b = texture(tex, v_texcoord - offset);
        color.r = r.r;
        color.b = b.b;
    }
    
    // Apply alpha
    color.a *= alpha;
    
    fragColor = color;
} 