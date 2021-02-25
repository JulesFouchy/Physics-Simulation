#version 430 core

uniform float u_invAspectRatio;

layout (location = 0) in vec2 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;
out vec3 vColor;

layout(std430, binding=0) buffer particlePositions {
   vec2 partPos[];
};

layout(std430, binding=2) buffer colors{
   float channel[];
};

void main(){
    vTexCoords = aTexCoords;
    vColor = vec3(1., 0.5, 0.3); //vec3(channel[3*gl_InstanceID],channel[3*gl_InstanceID+1],channel[3*gl_InstanceID+2]);
    vec2 pos = partPos[gl_InstanceID] + aVertexPos * 0.1;
    pos.x *= u_invAspectRatio;
    gl_Position = vec4(pos, 0.0, 1.0);
}