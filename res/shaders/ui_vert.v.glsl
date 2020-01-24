#version 110

uniform vec2 position;
uniform vec2 size;

attribute vec2 uv;

varying vec2 texcoord;

void main () {
    texcoord = uv;

    vec2 pos;
    pos.x = uv.x == 0. ? 
        position.x :
        position.x + size.x;

    pos.y = uv.y == 1. ?
        position.y :
        position.y - size.y;


    gl_Position = vec4 (pos, 0., 1.);
}