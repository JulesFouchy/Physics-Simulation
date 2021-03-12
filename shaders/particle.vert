#version 430 core

uniform mat4 _view_mat;
uniform mat4 _proj_mat;

layout (location = 0) in vec4 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main(){
    vTexCoords = aTexCoords;
    gl_Position = _proj_mat * _view_mat * aVertexPos;
}