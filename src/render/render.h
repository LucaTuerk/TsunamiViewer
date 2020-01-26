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
#include <iostream>
#include "../data/resources.h"
#include "../data/matrix.h"
#include "../data/netcdf/netcdfReader.h"
#include "../data/ui.h"
#include "../data/global_defines.h"

static void renderBackground () {
    // Set Program for use
    glUseProgram ( resources.program );
    
    // Set Uniforms
    glm::mat4 MVP = projection() * view() * earth_model();
    glUniformMatrix4fv ( resources.uniforms.MVP, 1, GL_FALSE, glm::value_ptr (MVP) );
    glUniform1i ( resources.uniforms.blurStrength, resources.blurStrength);

    // Set Textures
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, resources.textures[0] );
    glUniform1i ( resources.uniforms.textures[0], 0 );

    glActiveTexture ( GL_TEXTURE1 );
    glBindTexture (GL_TEXTURE_2D, resources.textures[1] );
    glUniform1i ( resources.uniforms.textures[1], 1 );

    // Set Buffers
    glBindBuffer ( GL_ARRAY_BUFFER, resources.vertex_buffer );
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

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, resources.element_buffer );
    glDrawElements (
        GL_TRIANGLES,                   // mode
        resources.count,  // count
        GL_UNSIGNED_SHORT,              // type
        (void *) 0                      // offset
    );

    glDisableVertexAttribArray( earth_resources.attributes.position );
    glDisableVertexAttribArray( earth_resources.attributes.uv );
}

static void setEarthUniforms () {
    // Set Uniforms
    glm::mat4 MVP = projection() * view() * earth_model();
    glUniformMatrix4fv ( earth_resources.uniforms.MVP, 1, GL_FALSE, glm::value_ptr (MVP) );
    glUniformMatrix4fv ( earth_resources.uniforms.M, 1, GL_FALSE, glm::value_ptr ( earth_model()) );
    glUniformMatrix4fv ( earth_resources.uniforms.V, 1, GL_FALSE, glm::value_ptr ( view() ) );
    glUniformMatrix4fv ( earth_resources.uniforms.P, 1, GL_FALSE, glm::value_ptr ( projection() ) );
    glUniformMatrix4fv ( earth_resources.uniforms.LRot, 1, GL_FALSE, glm::value_ptr ( lightingRot() ));
    glUniform4fv ( earth_resources.uniforms.viewDir, 1, glm::value_ptr(viewDir()));
    glUniform4fv ( earth_resources.uniforms.moonDir, 1, glm::value_ptr(moonDir()));
    glUniform4fv ( earth_resources.uniforms.sunDir, 1,  glm::value_ptr(sunDir()));
    glUniform1f ( earth_resources.uniforms.minVal, earth_resources.minVal );
    glUniform1f ( earth_resources.uniforms.maxVal, earth_resources.maxVal );
    glUniform1f ( earth_resources.uniforms.ambient, earth_resources.ambient );

    switch ( earth_resources.mode ) {
        case displayMode :: H :
            glUniform1ui ( earth_resources.uniforms.mode, modeH );
            glUniform1f ( earth_resources.uniforms.minVal, earth_resources.hMin);
            glUniform1f ( earth_resources.uniforms.maxVal, earth_resources.hMax);
            break;
        case displayMode :: U :
            glUniform1ui ( earth_resources.uniforms.mode, modeU );
            glUniform1f ( earth_resources.uniforms.minVal, earth_resources.huMin);
            glUniform1f ( earth_resources.uniforms.maxVal, earth_resources.huMax);
            break;
        case displayMode :: V :
            glUniform1ui ( earth_resources.uniforms.mode, modeV );
            glUniform1f ( earth_resources.uniforms.minVal, earth_resources.hvMin);
            glUniform1f ( earth_resources.uniforms.maxVal, earth_resources.hvMax);
            break;
        case displayMode :: UV :
            glUniform1ui ( earth_resources.uniforms.mode, modeUV );
            glUniform1f ( earth_resources.uniforms.minVal, earth_resources.huMin + earth_resources.hvMin );
            glUniform1f ( earth_resources.uniforms.maxVal, earth_resources.huMax + earth_resources.hvMax );
            break;
    }
}

static void setEarthBuffers () { 
    for ( int i = 0; i < earth_resources.vertex_buffers.size(); i++ ) {
        // Set Buffers
        glBindBuffer ( GL_ARRAY_BUFFER, earth_resources.vertex_buffers[i] );
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

        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, earth_resources.element_buffers[i] );
        glDrawElements (
            GL_TRIANGLES,                   // mode
            earth_resources.element_counts[i],  // count
            GL_UNSIGNED_SHORT,              // type
            (void *) 0                      // offset
        );

        glDisableVertexAttribArray( earth_resources.attributes.position );
        glDisableVertexAttribArray( earth_resources.attributes.uv );
    }
}

static void renderEarth () {
    // Set Program for use
    glUseProgram ( earth_resources.program );
    
    // Set Uniforms
    setEarthUniforms();

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

    glActiveTexture ( GL_TEXTURE4 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[4] );
    glUniform1i ( earth_resources.uniforms.textures[4], 4);

    // Set Buffers
    setEarthBuffers();
}

static float minimum ( float time, std::vector<float> & minV ) { 
    int step = earth_resources.reader->getTimeStep(time);
    if ( earth_resources.minMaxCalculated[ step ]) {
        return minV[step];
    }

    int dim = earth_resources.reader->getHeight() * earth_resources.reader->getWidth();
    float * ptr = & earth_resources.readBuffer[0];
    float mini = FLT_MAX;
    for ( int i = 0; i < dim; i++ ) {
        mini = mini < ptr[i] ? mini : ptr[i];
    }
    minV[step] = mini;
    return mini;
}

static float maximum ( float time, std::vector<float> & maxV  ) {
    int step = earth_resources.reader->getTimeStep(time);
    if ( earth_resources.minMaxCalculated[ step ]) {
        return maxV[step];
    }

    int dim = earth_resources.reader->getHeight() * earth_resources.reader->getWidth();
    float * ptr = & earth_resources.readBuffer[0];
    float maxi = FLT_MIN;
    for ( int i = 0; i < dim; i++ ) {
        maxi = maxi > ptr[i] ? maxi : ptr[i];
    }
    maxV[step] = maxi;
    return maxi;
}

static float minimumH ( float time, std::vector<float> & minV  ) {
    int step = earth_resources.reader->getTimeStep(time);
    if ( earth_resources.minMaxCalculated[ step ]) {
        return minV[step];
    }

    int dim = earth_resources.reader->getHeight() * earth_resources.reader->getWidth();
    float * ptr = & earth_resources.readBuffer[0];
    float * bPtr= & earth_resources.bBuffer[0];
    float mini = FLT_MAX;
    for ( int i = 0; i < dim; i++ ) {
        if (ptr[i] != 0. ) {
            float val =  ptr[i] + bPtr[i];
            mini = mini < val ? mini : val;
        }
    }
    minV[step] = mini;
    return mini;
}

static float maximumH ( float time, std::vector<float> & maxV ) {
    int step = earth_resources.reader->getTimeStep(time);
    if ( earth_resources.minMaxCalculated[ step ]) {
        return maxV[step];
    }

    int dim = earth_resources.reader->getHeight() * earth_resources.reader->getWidth();
    float * ptr = & earth_resources.readBuffer[0];
    float * bPtr= & earth_resources.bBuffer[0];
    float maxi = FLT_MIN;
    for ( int i = 0; i < dim; i++ ) {
        if ( ptr[i] != 0 ) {
            float val =  ptr[i] + bPtr[i];
            maxi = maxi > val ? maxi : val;
        }
    }
    maxV[step] = maxi;
    return maxi;
}


static void renderEarthFromData ( netcdfReader & reader, float time ) {
    // Set Program for use
    glUseProgram ( earth_resources.program );

    bool requires = reader.requiresUpdate( time );
    float * buffer = & earth_resources.readBuffer [0];
    // Set Textures b, h, hu, hv
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, earth_resources.textures[0] );
    if (requires) {

        glTexSubImage2D( 
            GL_TEXTURE_2D,                  // texture
            0,                              // lod level
            0,                              // x Offset
            0,                              // y Offset
            reader.getWidth(),              // width
            reader.getHeight(),             // height
            GL_RED,                         // format
            GL_FLOAT,                       // type
            &earth_resources.bBuffer[0]     // pointer
        ); 
    }

    glUniform1i ( earth_resources.uniforms.textures[0], 0 );

    glActiveTexture ( GL_TEXTURE1 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[1] );
    if (requires) {
        reader.writeBuffer(buffer, bufferType::H, reader.getTimeStep(time));  
        earth_resources.hMin = minimumH(time, earth_resources.hMinV);
        earth_resources.hMax = maximumH(time, earth_resources.hMaxV);

        glTexSubImage2D( 
            GL_TEXTURE_2D,                  // texture
            0,                              // lod level
            0,                              // x Offset
            0,                              // y Offset
            reader.getWidth(),              // width
            reader.getHeight(),             // height
            GL_RED,                        // format
            GL_FLOAT,                       // type
            &buffer[0]                      // pointer
        ); 
    }
    glUniform1i ( earth_resources.uniforms.textures[1], 1 );

    glActiveTexture ( GL_TEXTURE2 );
    glBindTexture ( GL_TEXTURE_2D, earth_resources.textures[2] );
    if ( requires ) {
        reader.writeBuffer(buffer, bufferType::HU, reader.getTimeStep(time));  
        earth_resources.huMin = minimum(time, earth_resources.huMinV);
        earth_resources.huMax = maximum(time, earth_resources.huMaxV);

        glTexSubImage2D( 
            GL_TEXTURE_2D,                  // texture
            0,                              // lod level
            0,                              // x Offset
            0,                              // y Offset
            reader.getWidth(),              // width
            reader.getHeight(),             // height
            GL_RED,                        // format
            GL_FLOAT,                       // type
            &buffer[0]                      // pointer
        ); 
    }

    glUniform1i ( earth_resources.uniforms.textures[2], 2 );

    glActiveTexture ( GL_TEXTURE3 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[3] );
    if ( requires ) {
        reader.writeBuffer(buffer, bufferType::HV, reader.getTimeStep(time));
        earth_resources.hvMin = minimum(time, earth_resources.hvMinV);
        earth_resources.hvMax = maximum(time, earth_resources.hvMaxV);

        glTexSubImage2D( 
            GL_TEXTURE_2D,                  // texture
            0,                              // lod level
            0,                              // x Offset
            0,                              // y Offset
            reader.getWidth(),              // width
            reader.getHeight(),             // height
            GL_RED,                        // format
            GL_FLOAT,                       // type
            &buffer[0]                      // pointer
        ); 
    }
    glUniform1i ( earth_resources.uniforms.textures[3], 3 );

    glActiveTexture ( GL_TEXTURE4 );
    glBindTexture (GL_TEXTURE_2D, earth_resources.textures[4] );
    glUniform1i ( earth_resources.uniforms.textures[4], 4);

    earth_resources.minMaxCalculated[
        earth_resources.reader->getTimeStep(time)
    ] = true;

    // Set Uniforms
    setEarthUniforms();

    // Set Buffers
    setEarthBuffers();
}

static void renderUI ( const std::vector<int> & elements ) {
    if (elements.size() == 0)
        return;

    // Set Program
    glUseProgram ( ui.program ); 
    // Set Buffers
    glBindBuffer ( GL_ARRAY_BUFFER, ui.uv_buffer );
    glVertexAttribPointer (
        ui.attributes.uv,                   // attribute
        2,                                  // size
        GL_FLOAT,                           // type
        GL_FALSE,                           // normalized
        sizeof(GLfloat)*2,                  // stride
        (void *) 0                          // offset
    );
    glEnableVertexAttribArray( ui.attributes.uv );
 
    glDepthFunc( GL_ALWAYS ); 
    glDisable( GL_CULL_FACE );

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, ui.index_buffer );

    for ( int e : elements ) {
        // Set Uniforms
        glUniform2fv ( ui.uniforms.position, 1, glm::value_ptr(ui.elements[e].position) );
        glUniform2fv ( ui.uniforms.size, 1, glm::value_ptr(ui.elements[e].size) );

        // Set Texture
        glActiveTexture ( GL_TEXTURE0 );
        glBindTexture ( GL_TEXTURE_2D, ui.elements[e].texture );
        glUniform1i ( ui.uniforms.texture, 0 );

        // Draw
        glDrawElements (
            GL_TRIANGLE_STRIP,              // mode
            4,                              // count
            GL_UNSIGNED_SHORT,              // type
            (void *) 0                      // offset
        );
    }

    glDisableVertexAttribArray( ui.attributes.uv );
}

static void renderProgressBar() {
    // Set Program for use
    glUseProgram ( progressbar.program );
    
    // Set Uniforms
    glUniform1f ( progressbar.uniforms.factor, earth_resources.currentTime );

    // Set Buffers
    glBindBuffer ( GL_ARRAY_BUFFER, progressbar.vertex_buffer );
    glVertexAttribPointer (
        progressbar.attributes.position,// attribute
        2,                                  // size
        GL_FLOAT,                           // type
        GL_FALSE,                           // normalized
        sizeof(GLfloat)*2,                  // stride
        (void *) 0                                   // offset
    );
    glEnableVertexAttribArray( progressbar.attributes.position );
 
    glDepthFunc( GL_ALWAYS ); 
    glDisable( GL_CULL_FACE );

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, progressbar.index_buffer );
    glDrawElements (
        GL_TRIANGLE_STRIP,                   // mode
        4,                              // count
        GL_UNSIGNED_SHORT,              // type
        (void *) 0                      // offset
    );

    glDisableVertexAttribArray( progressbar.attributes.position );
}

#endif