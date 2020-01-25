#version 140

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

const float hFac = 1.;
const float minHeight = -10035.;
const float maxHeight = 6877.;
const float offsetUv = 0.12;

float saturate ( float val ) {
    return min ( 1.0 , max ( 0.0, val) );
}

float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

float makeCombinedHeight ( float h, float b ) {
    float newB = b < 0. ? b : 0.1 * b;
    return map ( newB + h, minHeight, maxHeight, 0., 1. );
}

void main () {
    float y = map (uv.y, 0., 1., 0. - offsetUv, 1. + offsetUv);
    texcoord = vec2(uv.x, y);
    // Extend vertex along normal (normal = position on unit circle)
    b   =   textureLod( bTexture , texcoord, 0).r;
    h   =   textureLod( hTexture , texcoord, 0).r;
    bOver = map ( max ( .0, b), 0., maxHeight, 0., 1. );
    bPh =   makeCombinedHeight ( h, b );
    hu  =   textureLod( huTexture , texcoord, 0).r / 500.;
    hv  =   textureLod( hvTexture , texcoord, 0).r / 500.;
    
    vec3 pos = position.xyz * ( 1. + ( map ( ( b > 0. ? .01 * b : b ) + h, minHeight, maxHeight, 0., 1.) ) * hFac);

    normal = position.xyz;
    gl_Position = MVP * vec4(pos,1);
}