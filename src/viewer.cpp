#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "data/resources.h"
#include "data/structs.h"
#include "render/render.h"
#include "render/make.h"
#include "input/input.h"
#include "../res/res_defines.h"
#include "data/ui.h"

#include <memory>
#include <cmath>
#include <algorithm>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

static int makeResources (void) {
    camera.fov = 100.f;
    camera.distance = 5.f;

    camera.location = glm::vec3(0.,0.,5.);
    camera.theta = 0;

    earth_resources.reader = std::make_unique< netcdfReader > ("../res/netcdf/200122-sm_00.nc");
    if ( ! makeBackground ( NICE_SKY, STARS_TEX, VERT, FRAG ) )
        return 0;
    if ( ! makeEarth( earth_resources.reader->getHeight() / 2, earth_resources.reader->getWidth() / 2, EARTH_VERT, EARTH_FRAG) )
        return 0;
    if ( ! makeProgressbar ( PROGRESS_VERT, PROGRESS_FRAG) )
        return 0;
    if ( ! makeUI ( UI_VERT, UI_FRAG))
        return 0;

    elementIds.uibottom =
        makeUIElement ( glm::vec2 (-1., -0.80), glm::vec2 ( 2., 0.2), UIBOTTOM);
    elementIds.shadowbottom =
        makeUIElement ( glm::vec2 (-1., - 0.78), glm::vec2 (2., 0.1), EVENGREYER);
    elementIds.uitop =
        makeUIElement ( glm::vec2 ( -1., 1.), glm::vec2 ( 2., 0.05), GREY);
    elementIds.shadowtop =
        makeUIElement ( glm::vec2 ( -1., 0.985), glm::vec2 ( 2., 0.01), EVENGREYER);
    
    elementIds.play =
        makeUIElement ( glm::vec2 ( - 0.95, -0.8), glm::vec2 ( 0.1, 0.2), PLAY);
    elementIds.reverse =
        makeUIElement ( glm::vec2 ( - 0.95, -0.8), glm::vec2 ( 0.1, 0.2), REVERSE);
    elementIds.pause =
        makeUIElement ( glm::vec2 ( - 0.95, -0.8), glm::vec2 ( 0.1, 0.2), PAUSE);
    elementIds.stop =
        makeUIElement ( glm::vec2 ( - 0.95, -0.8), glm::vec2 ( 0.1, 0.2), STOP);
    elementIds.times1 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X1 );
    elementIds.times5 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X5 );
    elementIds.times10 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X10 );
    elementIds.times20 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X20 );
    elementIds.times50 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X50 );
    elementIds.times100 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X100 );
    elementIds.times200 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X200 );


    transportControl.timeSinceStart = 0;
    transportControl.state = transportState :: PAUSED;
    transportControl.multiplier = 20.f;
    return 1;
}

static void update (void) {
    // Fetch time
    int time = glutGet ( GLUT_ELAPSED_TIME );

    transportControl.deltaTime = time - transportControl.timeSinceStart;
    transportControl.timeSinceStart = time;

    int max = int (1000.f * earth_resources.reader->getMaxTime());

    if ( transportControl.state == transportState :: PLAYING )
        transportControl.time += transportControl.deltaTime * transportControl.multiplier;
    else if ( transportControl.state == transportState :: REVERSED )
        transportControl.time -= transportControl.deltaTime * transportControl.multiplier;
    else if 
        ( transportControl.state == transportState :: STOPPED ) 
            transportControl.time = 0;

    transportControl.time = std::clamp ( transportControl.time, 0, max);
    
    // Update aspect ratio
    camera.aspectRatio = (float) glutGet( GLUT_WINDOW_WIDTH ) / (float) glutGet( GLUT_WINDOW_HEIGHT );

    // Update time
    earth_resources.currentTime = ( (float) transportControl.time ) / ( 1000.f * earth_resources.reader->getMaxTime() );

    // Set Blur
    if ( std::abs(input.deltaMouseX) > 0 || std::abs(input.deltaMouseY) > 0)
        resources.blurStrength = 2;
    else
        resources.blurStrength = 0;

    // Update view
    updateCamera();

    glutPostRedisplay();
}

static void render (void) {
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    
    renderBackground ();
    renderEarthFromData ( * earth_resources.reader, earth_resources.currentTime);


    elementIds.list.push_back ( elementIds.shadowbottom );
    elementIds.list.push_back ( elementIds.uibottom );
    elementIds.list.push_back ( elementIds.shadowtop );
    elementIds.list.push_back ( elementIds.uitop );

    switch ( transportControl.multiplier ) {
        case 1 :
            elementIds.list.push_back(elementIds.times1);
            break;
        case 5 :
            elementIds.list.push_back( elementIds.times5 );
            break;
        case 10:
            elementIds.list.push_back( elementIds.times10 );
            break;
        case 20:
            elementIds.list.push_back( elementIds.times20 );
            break;
        case 50:
            elementIds.list.push_back( elementIds.times50 );
            break;
        case 100:
            elementIds.list.push_back( elementIds.times100 );
            break;
        case 200:
            elementIds.list.push_back( elementIds.times200 );
            break;
    }

    switch ( transportControl.state ) {
        case transportState :: PLAYING :
            elementIds.list.push_back ( elementIds.play );
            break;
        case transportState :: REVERSED :
            elementIds.list.push_back ( elementIds.reverse );
            break;
        case transportState :: PAUSED :
            elementIds.list.push_back ( elementIds.pause );
            break;
        case transportState :: STOPPED :
            elementIds.list.push_back ( elementIds.stop );
            break;
    }

    renderUI (elementIds.list);
    elementIds.list.clear();
    renderProgressBar();

    glutSwapBuffers ();
}


int main (int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(800, 640);
    glutCreateWindow("Tsunami Viewer");
    glutDisplayFunc(&render);
    glutMouseFunc(&mouseButtonFunc);
    glutPassiveMotionFunc(&mousePassive);
    glutMotionFunc(&mouseActive);
    glutKeyboardFunc(&keyboardFunc);
    glutSpecialFunc(&specialFunc);
    glutIdleFunc(&update);

    glewInit();
    if(!GLEW_VERSION_2_0) {
        std::cerr << "OpenGL 2 not available." << std::endl;
        return 1;
    }

    if(!makeResources()) {
        std::cerr << "Failed to load resources." << std::endl;
        return 1;
    }

    std::cout << "Entering Main Loop ->";
    glutMainLoop();
    std::cout << "<- Exiting Main Loop" << std::endl;
    return 0;
}