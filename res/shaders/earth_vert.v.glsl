#version 140

uniform mat4 MVP;

uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;

attribute vec4 position;
attribute vec2 uv;

varying vec2 texcoord;
varying vec3 normal;
//varying float bOver, bPh, b, h, hu, hv;

// Constants
const float hFac = .125;
const float minHeight = -10035.;
const float maxHeight = 6877.;
const float maxWaveHeight = 100.;
const float minWaveHeight = -100;
const float offsetUv = 0.15;

// Map value to new range
float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

void main () {
    // Setup uv to minimize stretch.
    // This leads to warping at the poles.
    float y     = map (uv.y, 0., 1., 0. - offsetUv, 1. + offsetUv);
    texcoord    = vec2(uv.x, y);

    // sample texures.
    float b       = texture2D( bTexture , texcoord).r;
    float h       = texture2D( hTexture , texcoord).r;
    
    // Extend vertex along normal (normal = position on unit circle)
    vec3 pos = position.xyz * ( 1.5 +  map((b + h), minHeight, maxHeight, 0., 1.) * hFac  );
    
    // Pass normal for lighting calculation
    normal = position.xyz;
    
    // Transform to clip space
    gl_Position = MVP * vec4(pos,1);
}