#version 430 core

in vec2 vTexCoords;
in vec3 vNormal;

uniform sampler2D _texture;

void main() {
    vec2 uv = vTexCoords;
    uv *= 10.;
    //uv = fract(uv);
    ivec2 gid = ivec2(floor(uv));
    vec3 col = (gid.x + gid.y) % 2 == 0 ? vec3(0.15) : vec3(0.85); //texture(_texture, vTexCoords).xyz;
    float t = clamp(dot(vNormal, normalize(vec3(-1, -1, -10))), 0., 1.);
    gl_FragColor = vec4(
        col * (t + 0.5),
    1.);
}