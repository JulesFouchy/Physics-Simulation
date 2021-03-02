#version 430 core

uniform float _invAspectRatio;
uniform float _size;
uniform vec2 _position;

layout (location = 0) in vec2 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main(){
    vTexCoords = aTexCoords;
    vec2 pos = _position + aVertexPos * _size;
    pos.x *= _invAspectRatio;
    gl_Position = vec4(pos, 0.0, 1.0);
}