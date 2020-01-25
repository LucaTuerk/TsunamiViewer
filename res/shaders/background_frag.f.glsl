#version 140

uniform float fade_factor;
uniform sampler2D textures[2];
uniform int blurStrength;

varying vec2 texcoord;

void main () {
    gl_FragColor = textureLod(textures[0], texcoord, blurStrength);
}