#version 140

// transformation matricies
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

// height textrues
uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;
uniform sampler2D nightTexture;

// lighting vectors
uniform vec3 viewDir;
uniform vec3 sunDir;
uniform vec3 moonDir;

// 
varying vec2 texcoord;
varying vec3 normal;
varying float bOver, b, h, hu, hv, random;

// lighting constants
const float ambientFac  = 0.3;
const float specularFac = 2.;
const vec3 sunColor     = vec3 ( 1.0, 0.85, 0.125 );
const vec3 moonColor    = vec3 ( 0.1, 0.125, 0.4 );

// color constants
const vec3 white        = vec3 (1.,1.,1.);
const vec3 grass        = 0.5 * vec3 ( .36, .38, .42);
const vec3 sand         = 0.5 * vec3(0.79, 0.66, 0.64);
const vec3 water        = 0.5 * vec3(0.58, 0.56, 0.66);
const vec3 darkWater    = 0.5 * vec3 (0.24, 0.25, 0.30);

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
    vec3 halfway = normalize ( lightDir + normalize ( viewDir ) );

    const float energy = ( 8. + specularFac ) / ( 8.0 * pi);
    float spec = energy * pow ( max ( dot ( normal, halfway), 0.0 ), isWater ? (4. * specularFac) : (specularFac) );
    return spec * lightColor;
} 

// Fragment function
void main () {
    bool isWater = ! (h == 0.);

    // Calculate main color
    vec3 color;
    if ( ! isWater )
        color = mix ( mix ( sand, grass, map ( texcoord.y, -1., 1., 0., 1.)), white, bOver );
    else
        color = mix (water, darkWater, map (h, 0., - minHeight + 100., 0., 1.));


    // Transform lighting vectors
    vec3 worldSunDir = normalize( ( MVP * vec4( sunDir, 1. ) ).xyz);
    vec3 worldMoonDir = normalize ( ( MVP * vec4( - moonDir, 1. ) ).xyz);
    vec3 normNormal = normalize ( normal );

    // Get ambient color
    vec3 ambientTerm = ambientFac * color;

    // Calculate diffuse term
    float diffFac = (1. - ambientFac) * 
        ( 0.5 * max ( dot ( worldSunDir, normNormal), 0.0 ) 
        + 0.5 *  max ( dot ( worldMoonDir, normNormal), 0.0 ));

    vec3 diffuseTerm = diffFac * color;

    // Calculate specular term form moon and sun light sources
    vec3 moonTerm =  0.50 * calculateSpecularTerm ( worldMoonDir, moonColor, normNormal, isWater);
    vec3 sunTerm =  0.50 * calculateSpecularTerm ( worldSunDir, sunColor, normNormal, isWater);

    // calculate wave speed
    float speed = sqrt(hu * hu + hv * hv);

    vec3 main = ambientTerm + diffuseTerm + moonTerm + sunTerm;
    if ( length (main) < 0.25 && !isWater) {
        main += 0.5 * textureLod ( nightTexture, texcoord  * nightTexStretch, 0 ).r;
    }

    // Set out color
    gl_FragColor = 
         vec4( 
         main +  
         + speed * vec3(1.,0.,0.) 
         , 1. );
}