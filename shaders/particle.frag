#version 430 core

in vec2 vTexCoords;
in vec3 vNormal;

void main() {
    vec3 col = vec3(vTexCoords, 0.);
    gl_FragColor = vec4(col, 1.);
}