#version 430 core

in vec2 vTexCoords;
in vec3 vNormal;

uniform sampler2D _texture;

void main() {
    vec3 col = texture(_texture, vTexCoords).xyz;
    gl_FragColor = vec4(col, 1.);
}