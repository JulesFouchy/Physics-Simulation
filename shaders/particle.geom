#version 430 core

uniform mat4 _view_mat;
uniform mat4 _proj_mat;

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in  vec2 gTexCoords[];
out vec2 vTexCoords;
out vec3 vNormal;

void main() {
    vec3 a = ( _proj_mat * _view_mat * gl_in[1].gl_Position - _proj_mat * _view_mat * gl_in[0].gl_Position ).xyz;
    vec3 b = ( _proj_mat * _view_mat * gl_in[2].gl_Position - _proj_mat * _view_mat * gl_in[0].gl_Position ).xyz;
    vec3 N = normalize( cross( b, a ) );

    for( int i=0; i<gl_in.length( ); ++i ) {
        gl_Position = _proj_mat * _view_mat * gl_in[i].gl_Position;
        vTexCoords = gTexCoords[i];
        vNormal = N;
        EmitVertex( );
    }

    EndPrimitive( );
}