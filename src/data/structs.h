#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "global_defines.h"


// Camera
static struct {
    glm::vec3 location, rotation;
    float fov;
    float aspectRatio;

    // position to earth
    float theta = M_PI / 2;
    float lambda = 0;
    float distance;
} camera;

// Background Rendering 
static const GLfloat vertex_buffer_data [] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f
};

static const GLushort element_buffer_data [] = {
    0, 1, 2, 3
};

static struct {
    GLuint vertex_buffer;
    GLuint element_buffer;
    GLuint textures[2];

    GLuint vertex_shader, fragment_shader, program;
    
    struct {
        GLint MVP;
        GLint textures[2];
    } uniforms;

    struct {
        GLint position;
        GLint uv;
    } attributes;

    GLfloat fade_factor;
} resources;


// Earth rendering
typedef struct {
    glm::vec4 position;
    glm::vec2 uv;
} vertex;

static struct {
    GLuint vertex_buffer;
    GLuint element_buffer;
    GLuint element_count;
    GLuint textures[4]; // b, h, hu, hv 

    // Transform
    glm::vec3 location = glm::vec3 (0,0,0); 
    glm::vec3 scale = glm::vec3 (1,1,1);
    glm::quat rotation = glm::angleAxis ( 0.f, UP);

    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint MVP;
        GLint time;
        GLint textures[4];
        GLint viewDir;
        GLint sunDir;
        GLint moonDir;
    } uniforms;

    struct {
        GLint position;
        GLint uv;
    } attributes;

    GLfloat time;
} earth_resources;

#endif
