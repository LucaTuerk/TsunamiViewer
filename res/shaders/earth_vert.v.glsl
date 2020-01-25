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
varying float bOver, bPh, b, h, hu, hv;

// Constants
const float hFac = 1.;
const float minHeight = -10035.;
const float maxHeight = 6877.;
const float offsetUv = 0.15;

// Map value to new range
float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

// Combine h and b values for overall height values. 
float makeCombinedHeight ( float h, float b ) {
    float newB = b < 0. ? b : 0.1 * b;
    return map ( newB + h, minHeight, maxHeight, 0., 1. );
}

void main () {
    // Setup uv to minimize stretch.
    // This leads to warping at the poles.
    float y     = map (uv.y, 0., 1., 0. - offsetUv, 1. + offsetUv);
    texcoord    = vec2(uv.x, y);

    // sample texures.
    b       = texture2D( bTexture , texcoord).r;
    h       = texture2D( hTexture , texcoord).r;
    bOver   = map ( max ( .0, b), 0., maxHeight, 0., 1. );
    bPh     = makeCombinedHeight ( h, b );
    hu      = texture2D( huTexture, texcoord).r;
    hv      = texture2D( hvTexture, texcoord).r;
    
    // Extend vertex along normal (normal = position on unit circle)
    vec3 pos = position.xyz * ( 1. + ( map ( ( b > 0. ? .01 * b : b ) + h, minHeight, maxHeight, 0., 1.) ) * hFac);
    
    // Pass normal for lighting calculation
    normal = position.xyz;
    
    // Transform to clip space
    gl_Position = MVP * vec4(pos,1);
}