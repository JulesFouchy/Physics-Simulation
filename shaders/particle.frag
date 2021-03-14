#version 430 core

in vec2 vTexCoords;
in vec3 vNormal;

void main(void){
    vec3 col = vNormal; //vec3(vTexCoords, 0.);
    gl_FragColor = vec4(col, 1.);
}