#version 430 core

in vec2 vTexCoords;
in vec3 vNormal;

uniform sampler2D _texture;

void main() {
    vec3 col = texture(_texture, vTexCoords).xyz;
    float t = clamp(dot(vNormal, normalize(vec3(1, 1, 10))), 0., 1.);
    gl_FragColor = vec4(
        col * (t + 0.5),
    1.);
}