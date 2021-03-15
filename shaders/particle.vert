#version 430 core

layout (location = 0) in vec4 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main(){
    vTexCoords = aTexCoords;
    gl_Position = aVertexPos;
}