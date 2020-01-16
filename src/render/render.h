#ifndef _RENDER_H
#define _RENDER_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../data/resources.h"
#include "../data/matrix.h"

static void renderBackground () {
    // Set Program for use
    glUseProgram ( resources.program );
    
    // Set Uniforms
    glm::mat4 MVP = projection() * view() * earth_model();
    glUniformMatrix4fv ( resources.uniforms.MVP, 1, GL_FALSE, glm::value_ptr (MVP) );

    // Set Textures
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, resources.textures[0] );
    glUniform1i ( resources.uniforms.textures[0], 0 );

    glActiveTexture ( GL_TEXTURE1 );
    glBindTexture (GL_TEXTURE_2D, resources.textures[1] );
    glUniform1i ( resources.uniforms.textures[1], 1 );

    // Set Buffers
    glBindBuffer ( GL_ARRAY_BUFFER, earth_resources.vertex_buffer );
    glVertexAttribPointer (
        resources.attributes.position,// attribute
        3,                                  // size
        GL_FLOAT,                           // type
        GL_FALSE,                           // normalized
        sizeof(vertex),                     // stride
        reinterpret_cast<void*> 
            (offsetof(vertex, position))    // offset
    );
    glEnableVertexAttribArray( resources.attributes.position );

    glVertexAttribPointer (
        resources.attributes.uv,        // attribute
        2,                              // size
        GL_FLOAT,                       // type
        GL_FALSE,                       // normalized
        sizeof(vertex),                 // stride
        reinterpret_cast<void*> 
            (offsetof(vertex, uv))      // offset
    );
    glEnableVertexAttribArray( resources.attributes.uv );

    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glPolygonMode( GL_FRONT, GL_FILL );
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc( GL_LESS ); 
    glDepthMask( GL_TRUE );

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, earth_resources.element_buffer );
    glDrawElements (
        GL_TRIANGLES,                   // mode
        earth_resources.element_count,  // count
        GL_UNSIGNED_SHORT,              // type
        (void *) 0                      // offset
    );

    glDisableVertexAttribArray( earth_resources.attributes.position );
    glDisableVertexAttribArray( earth_resources.attributes.uv );
}

static void renderEarth () {
    // Set Program for use
    glUseProgram ( earth_resources.program );
    
    // Set Uniforms
    glm::mat4 MVP = projection() * view() * earth_model();
    glUniform1f ( earth_resources.uniforms.time, earth_resources.time );
    glUniformMatrix4fv ( earth_resources.uniforms.MVP, 1, GL_FALSE, glm::value_ptr (MVP) );
    glUniform3fv ( earth_resources.uniforms.viewDir, 1, glm::value_ptr(viewDir()));
    glUniform3fv ( earth_resources.uniforms.moonDir, 1, glm::value_ptr(viewDir()));
    glUniform3fv ( earth_resources.uniforms.sunDir, 1, glm::value_ptr(viewDir()));

    // Set Textures
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, earth_resources.textures[0] );
    glUniform1i ( earth_resources.uniforms.textures[0], 0 );

    glActiveTexture ( GL_TEXTURE1 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[1] );
    glUniform1i ( earth_resources.uniforms.textures[1], 1 );

    glActiveTexture ( GL_TEXTURE2 );
    glBindTexture ( GL_TEXTURE_2D, earth_resources.textures[2] );
    glUniform1i ( earth_resources.uniforms.textures[2], 2 );

    glActiveTexture ( GL_TEXTURE3 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[3] );
    glUniform1i ( earth_resources.uniforms.textures[3], 3 );

    // Set Buffers
    glBindBuffer ( GL_ARRAY_BUFFER, earth_resources.vertex_buffer );
    glVertexAttribPointer (
        earth_resources.attributes.position,// attribute
        3,                                  // size
        GL_FLOAT,                           // type
        GL_FALSE,                           // normalized
        sizeof(vertex),                     // stride
        reinterpret_cast<void*> 
            (offsetof(vertex, position))    // offset
    );
    glEnableVertexAttribArray( earth_resources.attributes.position );

    glVertexAttribPointer (
        earth_resources.attributes.uv,  // attribute
        2,                              // size
        GL_FLOAT,                       // type
        GL_FALSE,                       // normalized
        sizeof(vertex),                 // stride
        reinterpret_cast<void*> 
            (offsetof(vertex, uv))      // offset
    );
    glEnableVertexAttribArray( earth_resources.attributes.uv );

    glFrontFace( GL_CW );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glPolygonMode( GL_FRONT, GL_FILL );
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc( GL_LESS ); 
    glDepthMask( GL_TRUE );

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, earth_resources.element_buffer );
    glDrawElements (
        GL_TRIANGLES,                   // mode
        earth_resources.element_count,  // count
        GL_UNSIGNED_SHORT,              // type
        (void *) 0                      // offset
    );

    glDisableVertexAttribArray( earth_resources.attributes.position );
    glDisableVertexAttribArray( earth_resources.attributes.uv );
}

#endif