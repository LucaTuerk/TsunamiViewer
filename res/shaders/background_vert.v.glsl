#version 110

attribute vec4 position;
attribute vec2 uv;

uniform mat4 MVP;

varying vec2 texcoord;

const float dist = 30.;

void main () {
    vec3 pos = position.xyz * dist;
    gl_Position = MVP * vec4(pos,1);
    texcoord = uv;
}