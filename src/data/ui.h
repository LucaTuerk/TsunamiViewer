#ifndef _UI_H
#define _UI_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "structs.h"
#include <vector>
#include <glm/glm.hpp>

typedef struct {
    glm::vec2 position;
    glm::vec2 size;
    GLuint texture;
} ui_element;

static struct {
    std::vector<ui_element> elements;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    GLuint uv_buffer;
    GLuint index_buffer;

    struct {
        GLuint position;
        GLuint size;
        GLuint texture;
    } uniforms; 

    struct {
        GLuint uv;
    } attributes;
} ui;

static struct {
    int help;
    int play, reverse, pause, stop;
    int uibottom, uitop, shadowbottom, shadowtop;
    int times1,
        times5,
        times10,
        times20,
        times50,
        times100,
        times200;
    std::vector<int> list;
} elementIds;

static struct {
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;

    struct {
        GLuint factor;
    } uniforms;

    struct {
        GLuint position;
    } attributes;
} progressbar;
#endif