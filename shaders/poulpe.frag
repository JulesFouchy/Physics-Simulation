#version 430 core

in vec2 vTexCoords;

uniform vec3 _body_color;
uniform vec3 _second_color;

void main() {
    if (length(vTexCoords - vec2(0.5)) > 0.5)
        discard;
    vec2 uv = vTexCoords;
    // Body 
    vec3 col = _body_color;
    // Mouth
    {
        float t = smoothstep(0., 0.001, length((uv - vec2(0.5, 0.3))* vec2(1.2, 1.)) - 0.08);
        col = mix(_second_color, col, t * 0.8);
    }
    // Symmetry
    uv -= 0.5;
    float sgn = sign(uv.x);
    uv.x = abs(uv.x);
    // Gradient
    {
        float t = smoothstep(0., 1., length(uv - vec2(0.1, 0.3)));
        col = mix(col, _second_color, t * 0.8);
    }
    // Eyes
    {
        uv -= vec2(0.2, 0.2);
        float t = smoothstep(0.001, -0.001, length(uv) - 0.05);
        col = mix(col, _second_color, t);
    }
    // Pupil
    {
        float t = smoothstep(0.001, -0.001, length(uv) - 0.04);
        col = mix(col, vec3(0.15), t);
    }
    // Reflection
    {
        float t = smoothstep(0.001, -0.001, length(uv - vec2(sgn * 0.011, 0.01)) - 0.02);
        col = mix(col, vec3(0.95), t);
    }
    // Eyebrows
    {
        float angle = -atan(uv.y, uv.x) / 6.28 + 0.5;
        float angle_delta = 0.1;
        if (angle > 0.25 - angle_delta && angle < 0.25 + angle_delta) {
            float weight = 0.01;
            float t = smoothstep(weight + 0.001, weight, abs(length(uv) - 0.08));
            col = mix(col, _second_color, t);
        }
    }
    gl_FragColor = vec4(col, 1.);
}