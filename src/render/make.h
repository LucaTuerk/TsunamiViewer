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
#include "../data/ui.h"
#include "../data/buffersplitter.h"
#include "../data/structs.h"

static float lerp (float a, float b, float t) {
    return (1-t) * a + t * b;
}

// Following http://www.songho.ca/opengl/gl_sphere.html
template < typename I >
static int makeUVSphere ( int nLat, int nLon, std::vector < vertex > & data, std::vector < I > & indices ) {
    std::cout << "Making UV " << (nLat + 1) * (nLon + 1) + 2 << std::endl;

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

    I k1, k2;
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

    return 1;
}

static int makeBackground ( const char * tex1, const char * tex2, const char * vert, const char * frag ) {
    std::cout << "Making Background Resources" << std::endl;

    std::vector < vertex > vertexB;
    std::vector < GLushort > indexB;

    if ( !makeUVSphere <GLushort> (200, 200,
            vertexB, indexB) ) 
        return 0;

    resources.vertex_buffer = makeBuffer ( 
            GL_ARRAY_BUFFER,
            (void *) &vertexB[0],
            vertexB.size() * sizeof( vertex )
        );

    resources.element_buffer = makeBuffer (
        GL_ELEMENT_ARRAY_BUFFER,
        (void *) &indexB[0],
        indexB.size() * sizeof( GLushort )
    );

    resources.count = indexB.size();

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

    resources.uniforms.blurStrength = 
        glGetUniformLocation ( resources.program, "blurStrength");

    resources.attributes.position =
        glGetAttribLocation ( resources.program, "position" );

    resources.attributes.uv =
        glGetAttribLocation ( resources.program, "uv" );
        

    return 1;
}

static int makeEarth ( int nLat, int nLon, const char * vert, const char * frag, std::string night) {
    std::cout << "Making Earth Resources" << std::endl; 
    // Make Geometry
    std::vector < vertex > vertexB;
    std::vector < GLuint > indexB;
    if ( !makeUVSphere <GLuint> (nLat, nLon, 
                    vertexB, indexB) ) 
        return 0;

    std::vector < std::vector < vertex > > vertBuffers;
    std::vector < std::vector < GLushort > > indBuffers;

    // Split buffers to allow for GLushort indexing
    splitBuffers ( vertexB, indexB, vertBuffers, indBuffers );

    for ( int i = 0; i < vertBuffers.size () && i < indBuffers.size (); i++ ) {
        earth_resources.vertex_buffers.push_back ( makeBuffer ( 
            GL_ARRAY_BUFFER,
            (void *) & (vertBuffers[i][0]),
            vertBuffers[0].size() * sizeof(vertex)
        ) );

        earth_resources.element_buffers.push_back ( makeBuffer (
            GL_ELEMENT_ARRAY_BUFFER,
            (void *) & (indBuffers[i][0]),
            indBuffers.at(i).size() * sizeof( GLushort )
        ) );

        earth_resources.element_counts.push_back ( indBuffers.at(0).size() );
    }
    

    // Make Textures
    earth_resources.textures[0] = makeTextureFromData ( bufferType :: B );
    earth_resources.textures[1] = makeTextureFromData ( bufferType :: H );
    earth_resources.textures[2] = makeTextureFromData ( bufferType :: HU );
    earth_resources.textures[3] = makeTextureFromData ( bufferType :: HV );
    earth_resources.textures[4] = makeTexture ( night );

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

    earth_resources.uniforms.M =
        glGetUniformLocation ( earth_resources.program, "M" );
    
    earth_resources.uniforms.V =
        glGetUniformLocation ( earth_resources.program, "V" );
    
    earth_resources.uniforms.P =
        glGetUniformLocation ( earth_resources.program, "P" );

    earth_resources.uniforms.viewDir =
        glGetUniformLocation ( earth_resources.program, "viewDir" );

    earth_resources.uniforms.moonDir =
        glGetUniformLocation ( earth_resources.program, "moonDir" );

    earth_resources.uniforms.sunDir =
        glGetUniformLocation ( earth_resources.program, "sunDir" );

    earth_resources.uniforms.mode =
        glGetUniformLocation ( earth_resources.program, "displayMode" );

    earth_resources.uniforms.minVal =
        glGetUniformLocation ( earth_resources.program, "minVal" );

    earth_resources.uniforms.maxVal =
        glGetUniformLocation ( earth_resources.program, "maxVal" );

    earth_resources.uniforms.textures[0] =
        glGetUniformLocation ( earth_resources.program, "bTexture" );
    
    earth_resources.uniforms.textures[1] =
        glGetUniformLocation ( earth_resources.program, "hTexture" );

    earth_resources.uniforms.textures[2] =
        glGetUniformLocation ( earth_resources.program, "hvTexture" );
    
    earth_resources.uniforms.textures[3] =
        glGetUniformLocation ( earth_resources.program, "huTexture" );

    earth_resources.uniforms.textures[4] =
        glGetUniformLocation ( earth_resources.program, "nightTexture" );

    earth_resources.attributes.position =
        glGetAttribLocation ( earth_resources.program, "position" );

    earth_resources.attributes.uv =
        glGetAttribLocation ( earth_resources.program, "uv" );


    earth_resources.readBuffer.reserve ( earth_resources.reader->getWidth() * earth_resources.reader->getHeight() );

    return 1;
}

static int makeProgressbar ( const char * vert, const char * frag ) {
    // Make Shaders
    progressbar.vertex_shader =
        makeShader (
            GL_VERTEX_SHADER,
            vert
        );
    if (progressbar.vertex_shader == 0) 
        return 0;

    progressbar.fragment_shader =
        makeShader (
            GL_FRAGMENT_SHADER,
            frag
        );
    if (progressbar.fragment_shader == 0)
        return 0;

    progressbar.program =
        makeProgram (
            {
                progressbar.vertex_shader, 
                progressbar.fragment_shader
            }
        );

    if (progressbar.program == 0)
        return 0;

    // Make Buffers
    progressbar.vertex_buffer = makeBuffer (
        GL_ARRAY_BUFFER,
        vertex_buffer_data,
        ( 8 * sizeof(GLfloat))
    );

    progressbar.index_buffer = makeBuffer (
        GL_ELEMENT_ARRAY_BUFFER,
        element_buffer_data,
        ( 4 * sizeof(GLushort))
    );

    // Make Uniforms
    progressbar.uniforms.factor = 
        glGetUniformLocation ( progressbar.program, "factor" );

    // Make Attributes
    progressbar.attributes.position =
        glGetAttribLocation ( progressbar.program, "position");

    return 1;
}

static int makeUI ( const char * vert, const char * frag ) {
    // Make Shaders
    ui.vertex_shader =
        makeShader (
            GL_VERTEX_SHADER,
            vert
        );
    if (ui.vertex_shader == 0) 
        return 0;

    ui.fragment_shader =
        makeShader (
            GL_FRAGMENT_SHADER,
            frag
        );
    if (ui.fragment_shader == 0)
        return 0;

    ui.program =
        makeProgram (
            {
                ui.vertex_shader, 
                ui.fragment_shader
            }
        );

    if (ui.program == 0)
        return 0;

    // Make Buffers
    ui.uv_buffer = makeBuffer (
        GL_ARRAY_BUFFER,
        uv_buffer_data,
        ( 8 * sizeof(GLfloat))
    );

    ui.index_buffer = makeBuffer (
        GL_ELEMENT_ARRAY_BUFFER,
        element_buffer_data,
        ( 4 * sizeof(GLushort))
    );

    // Make Uniforms
    ui.uniforms.position = 
        glGetUniformLocation ( ui.program, "position" );
    ui.uniforms.size = 
        glGetUniformLocation ( ui.program, "size" );

    // Make Attributes
    ui.attributes.uv =
        glGetAttribLocation ( ui.program, "uv");

    return 1;
}

static int makeUIElement ( glm::vec2 pos, glm::vec2 size, const char * texture ) {
    GLuint tex = makeTexture(texture);
    if ( tex == 0 )
        return 0;

    ui.elements.
        push_back ( {pos, size, tex});
    
    return ui.elements.size() - 1;
}

#endif