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
#include <iostream>
#include "../data/structs.h"

#define SPACEBAR 32

static struct {
    int mouseX, mouseY;
    int deltaMouseX, deltaMouseY;
    int mouseWheel;

    float sensitivity = 10.f;
    float scrollSensitivity = 0.5f;

    bool mouseClicked;
} input;

static void keyboardFunc ( unsigned char key, int x, int y ) {
    switch ( key ) {
        case SPACEBAR :
            transportControl.state = transportState :: PAUSED;
            break;
        case '1' :
            transportControl.multiplier = 1;
            break;
        case '2' :
            transportControl.multiplier = 5;
            break;
        case '3' :
            transportControl.multiplier = 10;
            break;
        case '4' :
            transportControl.multiplier = 20;
            break;
        case '5' :
            transportControl.multiplier = 50;
            break;
        case '6' :
            transportControl.multiplier = 100;
            break;
    }
}

static void specialFunc ( int key, int x, int y ) {
    switch ( key ) {
        case GLUT_KEY_LEFT :
            transportControl.state = transportState :: REVERSED;
            break;
        case GLUT_KEY_DOWN :
            transportControl.state = transportState :: STOPPED;
            break;
        case GLUT_KEY_RIGHT :
            transportControl.state = transportState :: PLAYING;
            break;
    }
}

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
    float distScaling = std::log ( camera.distance + 4.01f );
    camera.lambda += distScaling * input.sensitivity * (float) input.deltaMouseX / glutGet(GLUT_WINDOW_WIDTH);
    camera.theta += distScaling * input.sensitivity * (float) input.deltaMouseY  / glutGet(GLUT_WINDOW_HEIGHT);
    camera.distance = std::min ( 
            50.f, 
            std::max ( 
                -3.f, 
                camera.distance + 
                distScaling * input.scrollSensitivity * input.mouseWheel));
                input.mouseWheel = 0;
}

#endif