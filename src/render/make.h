#ifndef _MAKE_H
#define _MAKE_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include "../data/resources.h"

static float lerp (float a, float b, float t) {
    return (1-t) * a + t * b;
}

static int makeBackground ( const char * tex1, const char * tex2, const char * vert, const char * frag ) {
    std::cout << "Making Background Resources" << std::endl;
    // Make Buffers
    resources.vertex_buffer = makeBuffer ( 
        GL_ARRAY_BUFFER, 
        vertex_buffer_data, 
        sizeof(vertex_buffer_data)
    );

    resources.element_buffer = makeBuffer (
        GL_ELEMENT_ARRAY_BUFFER,
        element_buffer_data,
        sizeof(element_buffer_data)
    );

    // Make Textures
    resources.textures[0] = makeTexture ( tex1 );
    resources.textures[1] = makeTexture ( tex2 );

    if (resources.textures[0] == 0 || resources.textures[1] == 0)
        return 0;

    // Make Shaders
    resources.vertex_shader =
        makeShader (
            GL_VERTEX_SHADER,
            vert
        );
    if (resources.vertex_shader == 0) 
        return 0;

    resources.fragment_shader =
        makeShader (
            GL_FRAGMENT_SHADER,
            frag
        );
    if (resources.fragment_shader == 0)
        return 0;

    resources.program =
        makeProgram (
            {
                resources.vertex_shader, 
                resources.fragment_shader
            }
        );

    if (resources.program == 0)
        return 0;

    resources.uniforms.MVP =
        glGetUniformLocation ( resources.program, "MVP" );

    resources.uniforms.textures[0] =
        glGetUniformLocation ( resources.program, "textures[0]" );
    
    resources.uniforms.textures[1] =
        glGetUniformLocation ( resources.program, "textures[1]" );

    resources.attributes.position =
        glGetAttribLocation ( resources.program, "position" );

    resources.attributes.uv =
        glGetAttribLocation ( resources.program, "uv" );

    return 1;
}

// Following http://www.songho.ca/opengl/gl_sphere.html
static int makeUVSphere ( int nLat, int nLon ) {
    std::cout << "Making Earth" << std::endl;
    std::vector<vertex> data;
    std::vector<GLushort> indices;

    data.push_back( { glm::vec4 (0,1,0,1), glm::vec2(0,0) } );

    float lonStep = 1.f / (nLon);
    float latStep = 1.f / (nLat + 1.f);

    for ( int lat = 0; lat < nLat + 1; lat++) {
        for (int lon = 0; lon < nLon + 1; lon++) {

            glm::vec2 uv = 
                glm::vec2(lon * lonStep, 1.f - (lat + 1.f) * latStep);

            float theta = uv.x * 2.f * M_PI;
            float lambda = (uv.y - 0.5f) * M_PI;

            float c = cos(lambda);

            glm::vec4 pos =
                glm::vec4( c * cos(theta), sin(lambda), c * sin(theta), 1);


            uv.y = lerp (1,0,uv.y);
            data.push_back( {pos, uv} );
        }
    }

     data.push_back( { glm::vec4 (0,-1,0,1), glm::vec2(0,1) } );

    GLushort k1, k2;
    for (int i = 0; i < nLat + 1; i++) {
        k1 = i * ( nLon + 1 );
        k2 = k1 + nLon + 1; 
        for (int j = 0; j < nLon + 1; j++, k1++, k2++) {
            if ( i != 0 ) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back( (k1 + 1 ) );
            }
            if ( i != ( nLat ) ) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back( ( k2 + 1 ) );
            }
        }
    }

    std::cout << data.size() << " " << indices.size() << std::endl;

    earth_resources.vertex_buffer = makeBuffer (
        GL_ARRAY_BUFFER,
        (void *) &data[0],
        data.size() * sizeof(vertex)
    );

    earth_resources.element_buffer = makeBuffer (
        GL_ELEMENT_ARRAY_BUFFER,
        (void *) &indices[0],
        indices.size() * sizeof(GLushort)
    );

    earth_resources.element_count = indices.size();

    return 1;
}

static int makeCube () {
    return 1;
}

static int makeEarth ( int nLat, int nLon, const char * tex1, const char * tex2, const char * tex3, const char * tex4, const char * vert, const char * frag ) {
    std::cout << "Making Earth Resources" << std::endl; 
    // Make Geometry
    if ( !makeUVSphere (nLat, nLon) ) 
        return 0;

    // Make Textures
    earth_resources.textures[0] = makeTexture (tex1);
    earth_resources.textures[1] = makeTexture (tex2);
    earth_resources.textures[2] = makeTexture (tex3);
    earth_resources.textures[3] = makeTexture (tex4);

    if ( earth_resources.textures[0] == 0 || earth_resources.textures[1] == 0 || 
        earth_resources.textures[2] == 0 || earth_resources.textures[3] == 0 ) {
            return 0;
        }

    // Make Shaders
    earth_resources.vertex_shader =
        makeShader (
            GL_VERTEX_SHADER,
            vert
        );
    if (earth_resources.vertex_shader == 0) 
        return 0;

    earth_resources.fragment_shader =
        makeShader (
            GL_FRAGMENT_SHADER,
            frag
        );
    if (earth_resources.fragment_shader == 0)
        return 0;

    earth_resources.program =
        makeProgram (
            {
                earth_resources.vertex_shader, 
                earth_resources.fragment_shader
            }
        );

    if (earth_resources.program == 0)
        return 0;

    earth_resources.uniforms.MVP =
        glGetUniformLocation ( earth_resources.program, "MVP" );

    earth_resources.uniforms.time =
        glGetUniformLocation ( earth_resources.program, "time" );

    earth_resources.uniforms.viewDir =
        glGetUniformLocation ( earth_resources.program, "viewDir" );

    earth_resources.uniforms.moonDir =
        glGetUniformLocation ( earth_resources.program, "moonDir" );

    earth_resources.uniforms.sunDir =
        glGetUniformLocation ( earth_resources.program, "sunDir" );

    earth_resources.uniforms.textures[0] =
        glGetUniformLocation ( earth_resources.program, "bTexture" );
    
    earth_resources.uniforms.textures[1] =
        glGetUniformLocation ( earth_resources.program, "hTexture" );

    earth_resources.uniforms.textures[2] =
        glGetUniformLocation ( earth_resources.program, "hvTexture" );
    
    earth_resources.uniforms.textures[3] =
        glGetUniformLocation ( earth_resources.program, "huTexture" );

    earth_resources.attributes.position =
        glGetAttribLocation ( earth_resources.program, "position" );

    earth_resources.attributes.uv =
        glGetAttribLocation ( earth_resources.program, "uv" );

    return 1;
}

#endif