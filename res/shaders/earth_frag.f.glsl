#version 110

uniform float time;
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform sampler2D bTexture;
uniform sampler2D hTexture;
uniform sampler2D huTexture;
uniform sampler2D hvTexture;

uniform vec3 viewDir;
uniform vec3 sunDir;
uniform vec3 moonDir;

varying vec2 texcoord;
varying vec3 normal;
varying float bOver, b, h, hu, hv;

const float ambientFac = 0.45;
const float specularFac = 2.;
const vec3 sunColor = vec3 ( 1.0, 0.85, 0.125 );
const vec3 moonColor = vec3 ( 0.1, 0.125, 0.4 );

const vec3 white = vec3 (1.,1.,1.);
const vec3 grass= 0.5 * vec3 ( .36, .38, .42);
const vec3 sand = 0.5 * vec3(0.79, 0.66, 0.64);
const vec3 water = 0.5 * vec3(0.58, 0.56, 0.66);
const vec3 darkWater = 0.5 * vec3 (0.24, 0.25, 0.30);

const float pi = 3.14159265;
const float minHeight = -10035.;
const float maxHeight = 6877.;

float map ( float val, float minOrig, float maxOrig, float newMin, float newMax ) {
    return newMin + (val - minOrig) * (newMax - newMin) / (maxOrig - minOrig);
}

float sampleWaterTex ( int numIter, float step) {
    float sum;
    for ( int i = 1; i < numIter + 1; i++ ) {
        for (float x = -1.; x <= 1.; x+=1.) {
            for (float y = -1.; y <= 1.; y+=1.) {
                vec2 uv = texcoord + ( float(i) * step) * vec2 (x,y);
                float value = texture2D( hTexture, uv).r == 0. ? 0. : 1.;
                sum += value;
            }
        }
    } 
    return sum / ( 8. * float(numIter) );
}

vec3 calculateSpecularTerm ( vec3 lightDir, vec3 lightColor, vec3 normal, bool isWater) {
    vec3 halfway = normalize ( lightDir + normalize ( viewDir ) );

    const float energy = ( 8. + specularFac ) / ( 8.0 * pi);
    float spec = energy * pow ( max ( dot ( normal, halfway), 0.0 ), isWater ? (4. * specularFac) : (specularFac) );
    return spec * lightColor;
} 

void main () {
    bool isWater = !(h == 0.);

    vec3 color;
    if ( ! isWater )
        color = mix ( sand, mix ( grass, white, bOver), sampleWaterTex( 2, 0.1));
    else
        color = mix (water, darkWater, map (h, 0., - minHeight + 100., 0., 1.));


    vec3 worldSunDir = normalize( ( MVP * vec4( sunDir, 1. ) ).xyz);
    vec3 worldMoonDir = normalize ( ( MVP * vec4( - moonDir, 1. ) ).xyz);
    vec3 normNormal = normalize ( normal );

    // Get ambient color
    vec3 ambientTerm = ambientFac * color;

    // Get diffuse color
    float diffFac = (1. - ambientFac) * 
        ( 0.5 * max ( dot ( worldSunDir, normNormal), 0.0 ) 
        + 0.5 *  max ( dot ( worldMoonDir, normNormal), 0.0 ));

    vec3 diffuseTerm = diffFac * color;

    vec3 moonTerm =  0.50 * calculateSpecularTerm ( worldMoonDir, moonColor, normNormal, isWater);
    vec3 sunTerm =  0.50 * calculateSpecularTerm ( worldSunDir, sunColor, normNormal, isWater);

    float len = sqrt(hu * hu + hv * hv);

    gl_FragColor = 
        vec4( 
        ambientTerm + diffuseTerm + moonTerm + sunTerm
        // + map ( max ( h + minHeight, 0. ), 0. , 50., 0., 1.) * vec3(1.,0.,0.)
        , 1. );
}