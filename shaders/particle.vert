#version 430 core

uniform float _invAspectRatio;
uniform float _particle_size;
uniform bool _bPingPong;

layout (location = 0) in vec2 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;
out vec3 vColor;

layout(std430, binding=1) buffer particlePositions1 {
   vec2 partPos1[];
};
layout(std430, binding=2) buffer particlePositions2 {
   vec2 partPos2[];
};

layout(std430, binding=0) buffer colors{
   float channel[];
};

void main(){
    vTexCoords = aTexCoords;
    vColor = vec3(1., 0.5, 0.3); //vec3(channel[3*gl_InstanceID],channel[3*gl_InstanceID+1],channel[3*gl_InstanceID+2]);
    vec2 partPos = _bPingPong ? partPos1[gl_InstanceID] : partPos2[gl_InstanceID];
    vec2 pos = partPos + aVertexPos * _particle_size;
    pos.x *= _invAspectRatio;
    gl_Position = vec4(pos, 0.0, 1.0);
}