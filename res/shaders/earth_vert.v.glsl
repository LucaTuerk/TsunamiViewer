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
varying float bOver, bPh, b, h, hu, hv;

const float hFac = .125;
const float minHeight = -10035.;
const float maxHeight = 6877.;
const float offsetUv = 0.12;

float saturate ( float val ) {
    return min ( 1.0 , max ( 0.0, val) );
}

float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

void main () {
    float y = map (uv.y, 0., 1., 0. - offsetUv, 1. + offsetUv);
    texcoord = vec2(uv.x, y);
    // Extend vertex along normal (normal = position on unit circle)
    b   =   texture2D( bTexture , texcoord).r;
    h   =   texture2D( hTexture , texcoord).r;
    bOver = map ( max ( .0, b), 0., maxHeight, 0., 1. );
    bPh =   map ( b + h, minHeight, maxHeight, 0., 1. );
    hu  =   texture2D( huTexture , texcoord).r / 500.;
    hv  =   texture2D( hvTexture , texcoord).r / 500.;
    
    vec3 pos = position.xyz * ( 1. + ( bPh ) * hFac);

    normal = position.xyz;
    gl_Position = MVP * vec4(pos,1);
}