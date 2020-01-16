#ifndef _INPUT_H
#define _INPUT_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../data/structs.h"

static struct {
    int mouseX, mouseY;
    int deltaMouseX, deltaMouseY;
    int mouseWheel;

    float sensitivity = 10.f;
    float scrollSensitivity = 1.f;

    bool mouseClicked;
} input;

static void mouseButtonFunc ( int button, int state, int x, int y ) {
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
        input.mouseClicked = true;
    }
    else if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP ) {
        input.mouseClicked = false;
    }

    if ( button == 3 && state == GLUT_DOWN ) {
        input.mouseWheel = 1;
    }

    if ( button == 4  && state == GLUT_DOWN ) {
       input.mouseWheel = -1;
    }
}

static void mousePassive ( int x, int y ) {
    input.deltaMouseX = 0;
    input.deltaMouseY = 0;
    input.mouseX = x;
    input.mouseY = y;
}

static void mouseActive ( int x, int y ) {
    // Acitive func should only apply on left mouse clicks.
    if ( input.mouseClicked ) {
        input.deltaMouseX = x - input.mouseX;
        input.deltaMouseY = y - input.mouseY;
        input.mouseX = x;
        input.mouseY = y;
    }
}

static void updateCamera () {
    camera.lambda += input.sensitivity * (float) input.deltaMouseX / glutGet(GLUT_WINDOW_WIDTH);
    camera.theta += input.sensitivity * (float) input.deltaMouseY  / glutGet(GLUT_WINDOW_HEIGHT);
    camera.distance = std::min ( 50.f, std::max ( -3.f, camera.distance + input.scrollSensitivity * input.mouseWheel));
    input.mouseWheel = 0;
}

#endif