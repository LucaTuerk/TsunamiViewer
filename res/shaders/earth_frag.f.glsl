#version 140

// Display modes
const uint modeU  = uint( 0 );
const uint modeV  = uint( 1 );
const uint modeUV = uint( 2 );
const uint modeH  = uint( 3 );

// transformation matricies
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 LRot;

// max min values ( for selected value u, v, speed, )
uniform uint  displayMode;
uniform float minVal;
uniform float maxVal;

// height textures
uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;
uniform sampler2D nightTexture;

// lighting vectors
uniform vec4 viewDir;
uniform vec4 sunDir;
uniform vec4 moonDir;

// 
varying vec2 texcoord;
varying vec3 normal;
//varying float bOver, b, h, hu, hv, random;

// lighting constants
uniform float ambientFac;
const float specularFac = 2.;
const vec3 sunColor     = vec3 ( 1.0, 0.85, 0.125 );
const vec3 moonColor    = vec3 ( 0.1, 0.125, 0.4 );

// color constants
const vec3 white        = vec3 (1.,1.,1.);
const vec3 grass        = 0.5 * vec3 ( .36, .50, .42); // g: 0.38
const vec3 sand         = 0.5 * vec3(0.79, 0.66, 0.64);
const vec3 water        = 0.5 * vec3(0.58, 0.56, 0.88); // b: 0.66
const vec3 darkWater    = 0.5 * vec3 (0.24, 0.25, 0.50); // b: 0.30

// geometric constants
const float pi          = 3.14159265;
const float minHeight   = -10035.;
const float maxHeight   = 6877.;
const vec2 nightTexStretch = vec2 (1., 0.935);

// map value to new range
float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

// Calculate specular lighting term
vec3 calculateSpecularTerm ( vec3 lightDir, vec3 lightColor, vec3 normal, bool isWater) {
    vec3 halfway = normalize ( lightDir + normalize ( viewDir ).xyz );

    const float energy = ( 8. + specularFac ) / ( 8.0 * pi);
    float spec = energy * pow ( max ( dot ( normal, halfway), 0.0 ), isWater ? (4. * specularFac) : (specularFac) );
    return spec * lightColor;
} 

// Fragment function
void main () {
    // sample texures.
    float b       = texture2D( bTexture , texcoord).r;
    float h       = texture2D( hTexture , texcoord).r;
    float hu      = texture2D( huTexture, texcoord).r;
    float hv      = texture2D( hvTexture, texcoord).r;
    float bOver   = map ( max ( .0, b), 0., maxHeight, 0., 1. );

    bool isWater = ! (h == 0.);

    // Calculate main color
    vec3 color;
    if ( ! isWater )
        color = mix ( mix ( sand, grass, map ( texcoord.y, -1., 1., 0., 1.)), white, bOver );
    else
        color = mix (water, darkWater, map (h, 0., - minHeight + 100., 0., 1.));


    // Transform lighting vectors
    vec3 worldSunDir = normalize( ( LRot * M * sunDir ).xyz);
    vec3 worldMoonDir = normalize( ( LRot * M * LRot * moonDir ).xyz);
    vec3 normNormal = normalize ( normal );

    // Get ambient color
    vec3 ambientTerm = ambientFac * color;

    // Calculate diffuse term
    float diffRaw = 
        ( 0.5 * max ( dot ( worldSunDir, normNormal), 0.0 ) 
        + 0.5 *  max ( dot ( worldMoonDir, normNormal), 0.0 ));
    float diffFac = (1. - ambientFac) * diffRaw;

    vec3 diffuseTerm = diffFac * color;

    // Calculate specular term form moon and sun light sources
    vec3 moonTerm =  0.5 * calculateSpecularTerm ( worldMoonDir, moonColor, normNormal, isWater);
    vec3 sunTerm =  0.5 * calculateSpecularTerm ( worldSunDir, sunColor, normNormal, isWater);

    float dispFactor =
        displayMode == modeU    ? map   ( hu, minVal, maxVal, -1., 1. ) :
        displayMode == modeV    ? map   ( hv, minVal, maxVal, -1., 1. ) :
        displayMode == modeUV   ? map   ( hu + hv, minVal, maxVal, -1., 1. ) :
        displayMode == modeH    ? map   ( h , minVal, maxVal, -1., 1. )
         : 0.;

    vec3 displayTerm = abs ( minVal - maxVal ) < 0.5 ? vec3 (0.,0.,0.) : max ( 0.0, dispFactor ) * vec3 (0., 0., 1.) - min ( 0.0, dispFactor ) * vec3 (1.,0.,0.); 

    vec3 main = mix ( diffuseTerm + moonTerm + sunTerm + ambientTerm, displayTerm, isWater ? length ( displayTerm ) : 0. );

    if ( diffRaw < 0.30 && !isWater) {
        main += mix ( 
            vec3(0.),
            textureLod ( nightTexture, texcoord  * nightTexStretch, 0 ).rgb, ( 0.30 - diffRaw ) / 0.30
        );
    }

    // Set out color
    gl_FragColor = 
         vec4( 
         main
         , 1. );
}