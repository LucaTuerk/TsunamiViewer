#version 110

uniform float time;

uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;

uniform vec3 viewDir;
uniform vec3 sunDir;
uniform vec3 moonDir;

varying vec2 texcoord;
varying vec3 normal;

void main () {
    gl_FragColor = vec4( texture2D ( hTexture, texcoord ).rgb, 1 );
}