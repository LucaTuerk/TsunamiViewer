#version 110

uniform mat4 MVP;
uniform float time;

uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;

attribute vec4 position;
attribute vec2 uv;

uniform vec3 viewDir;
uniform vec3 sunDir;
uniform vec3 moonDir;

varying vec2 texcoord;
varying vec3 normal;

const float hFac = 0.08;

void main () {
    // Extend vertex along normal (normal = position on unit circle)
    float height = texture2D( bTexture , uv).r;
    
    vec3 pos = position.xyz * ( 1. + height * hFac);

    texcoord = uv;
    normal = position.xyz;
    gl_Position = MVP * vec4(pos,1);
}