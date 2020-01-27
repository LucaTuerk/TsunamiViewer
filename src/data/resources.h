#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <string>
#include <iostream>
#include <fstream>
#include "structs.h"
#include "../utils/TGA/tga.hpp"
#include "../data/netcdf/netcdfReader.h"


/**
 * Get content from file
 * 
 * @param file path to file
 * @param length out length of read data
 * @return char buffer with contents
 **/
static char * getFileContents ( const char * file, GLint * length ) {
    std::ifstream i_file;
    i_file.open ( file );

    if (i_file.fail()) {
        std::cerr << "Failed to open file " << file << std::endl;
        return 0;
    }

    i_file.seekg(0, i_file.end);
    int len = i_file.tellg();

    if (len == -1) {
        std::cerr << "tellg failed on input file " << file << std::endl;
        i_file.close ();
        return 0;
    }
    i_file.seekg(0, i_file.beg);

    char * buffer = new char[len];
    i_file.read (buffer, len);
    i_file.close ();

    * length = len;
    return buffer;
}


/**
 * Show shader error messages.
 **/
static void showInfoLog (GLuint object, PFNGLGETSHADERIVPROC glGetIV, PFNGLGETSHADERINFOLOGPROC glGetInfo) {
    GLint log_length;
    char * log;

    glGetIV(object, GL_INFO_LOG_LENGTH, &log_length);
    log = new char[log_length];
    glGetInfo(object, log_length, NULL, log);
    
    std::cerr << log;
    delete( log );
}


/**
 * Make buffer.
 * 
 * @param target type of buffer
 * @param bufferData pointer to data
 * @param bufferSize size of data
 **/
static GLuint makeBuffer ( GLenum target, const void * bufferData, GLsizei bufferSize ) {
    GLuint buffer;
    std::cout << "  Making Buffer: " << buffer << std::endl;
    glGenBuffers( 1, & buffer );
    glBindBuffer( target, buffer);
    glBufferData( target, bufferSize, bufferData, GL_STATIC_DRAW );
    return buffer;
}


/**
 * Generate Texture and set parameters.
 **/
static GLuint makeTexture (  ) {
    GLuint texture;
    // Generate and Bind Texture
    glGenTextures (1, &texture);
    glBindTexture (GL_TEXTURE_2D, texture);

    // Set Texture Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    return texture;
}


/**
 * Load an make a texture.
 * 
 * @param file path to image file
 **/
static GLuint makeTexture ( const std::string & file ) {
    GLuint texture = makeTexture();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    tga::TGA * img = new tga::TGA();
    std::cout << "  Loading Texture: " << file << std::endl;
    if(!img->Load(file))
        return 0;

    glTexImage2D(
        GL_TEXTURE_2D, 0,   // Target, LOD
        GL_RGB8,            // internal format
        img->GetWidth(), img->GetHeight(), 0,    // width, height, border
        GL_BGR, GL_UNSIGNED_BYTE,    // external format, type
        img->GetData()
    );

    delete(img);

    return texture;
}


/**
 * Make a texture filled with data from netCDF file
 * 
 * @param type type of data
 **/
static GLuint makeTextureFromData ( bufferType type ) {
    GLuint texture = makeTexture();

    float buffer[ earth_resources.reader->getWidth() * earth_resources.reader->getHeight()];
    if ( type == bufferType :: B) {
        earth_resources.bBuffer.reserve (earth_resources.reader->getWidth() * earth_resources.reader->getHeight());
        earth_resources.reader-> writeBuffer ( & earth_resources.bBuffer[0], type, 0);
    } else {
        earth_resources.reader-> writeBuffer ( buffer, type, 0 );
    }

    glTexImage2D (
        GL_TEXTURE_2D, 0, 
        GL_R32F,
        earth_resources.reader->getWidth(), earth_resources.reader->getHeight(), 0,
        GL_RED, GL_FLOAT,
        type == bufferType :: B ? & earth_resources.bBuffer[0] : buffer
    );

    return texture;
}


/**
 * Compile the passed shader.
 * 
 * @param type the type of shader to be compiled
 * @param file file path to shader file
 **/
static GLuint makeShader ( GLenum type, const char * file ) {
    GLint length;
    GLchar * source = getFileContents (file, &length);
    GLuint shader;
    GLint shader_ok;

    if ( !source )
        return 0;

    shader = glCreateShader (type);
    glShaderSource (shader, 1, (const GLchar **) &source, &length );
    free(source);

    std::cout << "  Compiling Shader: " << shader << std::endl;

    glCompileShader (shader);
    glGetShaderiv (shader, GL_COMPILE_STATUS, &shader_ok);

    if (!shader_ok) {
        std::cerr << "Failed to compile shader " << file << std::endl;
        showInfoLog(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


/**
 * Combine shaders to make a shader program.
 * 
 * @param shaders list of shaders to be combined
 **/
static GLuint makeProgram ( std::initializer_list<GLuint> shaders ) {
    GLint program_ok;

    GLuint program = glCreateProgram();

    std::cout << "  Linking Program: " << program << std::endl;

    for ( GLuint shader : shaders) {
        std::cout << "    Attaching Shader: " << shader << std::endl;
        glAttachShader(program, shader);
    }

    glLinkProgram (program);

    glGetProgramiv (program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        std::cerr << "Failed to link shader program" << std::endl;
        showInfoLog (program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram (program);
        return 0;
    }
    return program;
}

#endif