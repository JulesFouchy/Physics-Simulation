#version 430 core

layout (location = 0) in vec4 aVertexPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 _view_mat;
uniform mat4 _proj_mat;

out vec2 gTexCoords;

void main() {
    gTexCoords = aTexCoords;
    gl_Position = _proj_mat * _view_mat* aVertexPos;
}