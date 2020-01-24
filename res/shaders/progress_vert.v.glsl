#version 110
attribute vec2 position;

uniform float factor;

const float height = 0.015;

void main () {
    vec2 pos = position;
    pos.x = pos.x == 1. ? (-1. + (2. * factor)) : pos.x;
    pos.y = pos.y == -1.? 1. - height : pos.y;

    gl_Position = vec4 (pos, 0., 1.);
}