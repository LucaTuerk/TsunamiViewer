#version 110

uniform float fade_factor;
uniform sampler2D textures[2];
uniform int blurStrength;

varying vec2 texcoord;

vec4 blurTex ( int numIter, float step) {
    vec4 sum = vec4(0);
    sum += texture2D( textures[0], texcoord );
    for ( int i = 1; i < numIter + 1; i++ ) {
        for (float x = -1.; x <= 1.; x+=1.) {
            for (float y = -1.; y <= 1.; y+=1.) {
                vec2 uv = texcoord + ( float(i) * step ) * vec2 (x,y);
                sum += texture2D( textures[0], uv);
            }
        }
    } 
    float fac = 1. / ( 8. * float(numIter) + 1.);
    return fac * sum;
}

void main () {
    gl_FragColor = blurTex ( blurStrength, 0.001);
}