#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <memory>
#include <glm/gtc/quaternion.hpp>
#include "global_defines.h"
#include "netcdf/netcdfReader.h"
#include <vector>



/**
 * Struct for camera and lighting parameters.
 **/
static struct {
    glm::vec3 location, rotation;
    float fov;
    float aspectRatio;

    // position to earth
    float theta = M_PI / 2;
    float lambda = 0;
    float lightingLambda;
    float distance;
} camera;


/**
 * Vertex buffer for static background (unused)
 **/
static const GLfloat vertex_buffer_data [] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f
};


/**
 * buffer data of uv coords for ui
 * (vertex pos is determined in shader)
 **/
static const GLfloat uv_buffer_data [] = {
    0.f, 0.f,
    1.0f, 0.f,
    0.f, 1.0f,
    1.0f, 1.0f
};


/**
 * Index buffer data for UI
 **/
static const GLushort element_buffer_data [] = {
    0, 1, 2, 3
};


/**
 * Struct of resources for background rendering.
 **/
static struct {
    GLuint vertex_buffer;
    GLuint element_buffer;
    GLuint count;
    GLuint textures[2];

    GLuint vertex_shader, fragment_shader, program;
    
    struct {
        GLint MVP;
        GLint textures[2];
        GLint blurStrength;
    } uniforms;

    struct {
        GLint position;
        GLint uv;
    } attributes;

    GLint blurStrength;
} resources;



/**
 * vertex data structure for earth vertex buffer
 **/
typedef struct {
    glm::vec4 position;
    glm::vec2 uv;
} vertex;


/**
 * Enum of possible transport states
 **/
enum class transportState { PLAYING, REVERSED, PAUSED, STOPPED };


/**
 * struct of data for transport control
 **/
static struct {
    transportState state;
    int deltaTime;
    int time;
    int timeSinceStart;
    int multiplier;
} transportControl;


/**
 * Enum of possible visualization modes 
 **/
enum class displayMode { U, V, UV, H, NONE};

/**
 * Struct of resources for earth rendering.
 **/
static struct {
    std::unique_ptr<netcdfReader> reader;
    float currentTime;

    std::vector < GLuint > vertex_buffers;
    std::vector < GLuint > element_buffers;
    std::vector < GLuint > element_counts;

    std::vector<float> readBuffer;
    std::vector<float> bBuffer;

    GLuint textures[4]; // b, h, hu, hv 

    // Transform
    glm::vec3 location = glm::vec3 (0,0,0); 
    glm::vec3 scale = glm::vec3 (1,1,1);
    glm::quat rotation = glm::angleAxis ( 0.f, UP);

    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint MVP;
        GLint M;
        GLint V;
        GLint P;
        GLint LRot;
        GLint textures[5];
        GLint viewDir;
        GLint sunDir;
        GLint moonDir;
        GLint mode;
        GLint minVal;
        GLint maxVal;
        GLint ambient;
    } uniforms;

    struct {
        GLint position;
        GLint uv;
    } attributes;

    displayMode mode;
    GLfloat minVal;
    GLfloat maxVal;
    
    GLfloat hMin, hMax;
    GLfloat huMin, huMax;
    GLfloat hvMin, hvMax;
    GLfloat ambient;

    std::vector <float> hMinV, hMaxV, huMinV, huMaxV, hvMinV, hvMaxV;
    std::vector <bool> hCalc, hvCalc, huCalc;
    bool globalMinMax;
} earth_resources;

#endif
