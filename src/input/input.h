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


/**
 * Struct holding command line parameters and default values.
 **/
static struct {
    std::string filePath = "../res/netcdf/200122-sm_00.nc";
    int divisor = 1;
} commandLine;


/**
 * Struct holding input data.
 **/
static struct {
    int mouseX, mouseY;
    int deltaMouseX, deltaMouseY;
    int mouseWheel;

    float sensitivity = 10.f;
    float scrollSensitivity = 0.5f;

    bool mouseClicked;
    bool ambientUp, ambientDown, lightLeft, lightRight;
    bool showHelp;
} input;


/**
 * Mouse down listener.
 * Setting transport controls, display modes, lighting.
 **/
static void keyboardFunc ( unsigned char key, int x, int y ) {
    switch ( key ) {
        case SPACEBAR :
            transportControl.state = 
                transportControl.state == transportState :: PAUSED ?
                transportState :: PLAYING
                : transportState :: PAUSED;
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
        case '7' :
            transportControl.multiplier = 200;
            break;
        case '8' :
            transportControl.multiplier = 400;
            break;
        case '9' :
            transportControl.multiplier = 750;
            break;
        case 'u' :
            earth_resources.mode = displayMode :: U;
            break;
        case 'v' :
            earth_resources.mode = displayMode :: V;
            break;
        case 'h' :
            earth_resources.mode = displayMode :: H;
            break;
        case 'c' :
            earth_resources.mode = displayMode :: UV;
            break;
        case 'n' :
            earth_resources.mode = (displayMode) -1;
            break;
        case 'w' :
            input.ambientUp = true;
            break;
        case 's' :
            input.ambientDown = true;
            break;
        case 'a' :
            input.lightLeft = true;
            break;
        case 'd' :
            input.lightRight = true;
            break;
        case 'g' :
            earth_resources.globalMinMax = ! earth_resources.globalMinMax;
            break;
        case 27 : // Escape key
            input.showHelp = true;
            break;
    }
}


/**
 * Listener for released keys. 
 * Track escape key.
 **/
static void keyboardUpFunc ( unsigned char key, int x, int y ) {
    switch ( key ) {
        case 27 : // Escape key
            input.showHelp = false;
            break;
    }
}


/**
 * Listener for special character input. 
 * Necessary to read arrow keys.
 **/
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


/**
 * Function listening for mouse button input.
 * Track mouse buttons and wheels.
 **/
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


/**
 * Listener function called when the mouse is moved without a mouse button being pressed.
 * Used to set x, y values to calculate deltas in mouseActive, and to reset delta values.
 **/
static void mousePassive ( int x, int y ) {
    input.deltaMouseX = 0;
    input.deltaMouseY = 0;
    input.mouseX = x;
    input.mouseY = y;
}


/**
 * Listener function called when the mouse is moved and a mouse button is pressed.
 * Updates delta mouse movement to be used in updateCamera method.
 **/
static void mouseActive ( int x, int y ) {
    // Acitive func should only apply on left mouse clicks.
    if ( input.mouseClicked ) {
        input.deltaMouseX = x - input.mouseX;
        input.deltaMouseY = y - input.mouseY;
        input.mouseX = x;
        input.mouseY = y;
    }
}

/**
 * Update the camera according to input.
 **/
static void updateCamera () {
    float distScaling = std::log ( camera.distance + 4.01f );
    camera.lambda += distScaling * input.sensitivity * (float) input.deltaMouseX / glutGet(GLUT_WINDOW_WIDTH);
    camera.theta += distScaling * input.sensitivity * (float) input.deltaMouseY  / glutGet(GLUT_WINDOW_HEIGHT);
    camera.distance = std::min ( 
            50.f, 
            std::max ( 
                1.f, 
                camera.distance + 
                distScaling * input.scrollSensitivity * input.mouseWheel));
                input.mouseWheel = 0;
}

#endif