#version 430 core

in vec2 vTexCoords;

void main(void){
    vec3 col = vec3(vTexCoords, 0.);
    gl_FragColor = vec4(col, 1.);
}