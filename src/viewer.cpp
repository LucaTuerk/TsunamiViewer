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
#include <string>

static int makeResources (void) {
    camera.fov = 100.f;
    camera.distance = 5.f;

    camera.location = glm::vec3(0.,0.,5.);
    camera.theta = 0;

    earth_resources.reader = std::make_unique< netcdfReader > (commandLine.filePath.c_str());
    
    earth_resources.minMaxCalculated = std::vector<bool> ( 
        earth_resources.reader->getMaxTimeStep(), false
    );
    earth_resources.hMinV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MAX );
    earth_resources.hMaxV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MIN );
    earth_resources.huMinV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MAX );
    earth_resources.huMaxV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MIN );
    earth_resources.hvMinV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MAX );
    earth_resources.hvMaxV = std::vector<float> ( earth_resources.reader->getMaxTimeStep(), FLT_MIN );

    earth_resources.ambient = 0.45;

    if ( ! makeBackground ( NICE_SKY, STARS_TEX, VERT, FRAG ) )
        return 0;
    if ( ! makeEarth( earth_resources.reader->getHeight() / commandLine.divisor, earth_resources.reader->getWidth() / commandLine.divisor, EARTH_VERT, EARTH_FRAG, NIGHT) )
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
    elementIds.times400 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X400 );
    elementIds.times750 = 
        makeUIElement( glm::vec2 ( 0.85, -0.8 ), glm::vec2 ( 0.1, 0.2 ), X750 );
    elementIds.H = 
        makeUIElement( glm::vec2 ( -0.8, -0.825), glm::vec2 (0.15,0.15), HTEX);
    elementIds.U = 
        makeUIElement( glm::vec2 ( -0.8, -0.825), glm::vec2 (0.15,0.15), HUTEX);
    elementIds.V = 
        makeUIElement( glm::vec2 ( -0.8, -0.825), glm::vec2 (0.15,0.15), HVTEX);
    elementIds.UV = 
        makeUIElement( glm::vec2 ( -0.8, -0.825), glm::vec2 (0.15,0.15), HUHVTEX);
    elementIds.global =
        makeUIElement( glm::vec2 ( -0.65, -0.843), glm::vec2 ( 0.24, 0.12), GLOBAL );
    elementIds.local =
        makeUIElement( glm::vec2 ( -0.65, -0.843), glm::vec2 ( 0.24, 0.12), LOCAL );
    elementIds.help =
        makeUIElement ( glm::vec2(-1,1), glm::vec2 (2,2), HELP );


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

    // Update transport
    if ( transportControl.state == transportState :: PLAYING )
        transportControl.time += transportControl.deltaTime * transportControl.multiplier;
    else if ( transportControl.state == transportState :: REVERSED )
        transportControl.time -= transportControl.deltaTime * transportControl.multiplier;
    else if 
        ( transportControl.state == transportState :: STOPPED ) 
            transportControl.time = 0;

    transportControl.time = std::clamp ( transportControl.time, 0, max);
    
    // Update lighting
    if ( input.ambientUp ) {
        earth_resources.ambient = std::min ( 1., 
                    earth_resources.ambient + (float) transportControl.deltaTime / 2000. );
        input.ambientUp = false;
    }
    if ( input.ambientDown ) {
        earth_resources.ambient = std::max ( 0., 
            earth_resources.ambient - (float) transportControl.deltaTime / 2000. );
        input.ambientDown = false;
    }
    if ( input.lightLeft ) {
        camera.lightingLambda += (float) transportControl.deltaTime / 2000.;
        input.lightLeft = false;
    }
    if ( input.lightRight ) {
        camera.lightingLambda -= (float) transportControl.deltaTime / 2000.;
        input.lightRight = false;
    }
    
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
    renderEarthFromData ( * earth_resources.reader, earth_resources.currentTime );

    elementIds.list.push_back ( elementIds.shadowbottom );
    elementIds.list.push_back ( elementIds.shadowtop );

    if ( input.showHelp ) {
        elementIds.list.push_back ( elementIds.help );
    }

    elementIds.list.push_back ( elementIds.uibottom );
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
         case 400:
            elementIds.list.push_back( elementIds.times400 );
            break;
         case 750:
            elementIds.list.push_back( elementIds.times750 );
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

    switch ( earth_resources.mode ) {
        case displayMode :: H :
            elementIds.list.push_back ( elementIds.H );
            break;
        case displayMode :: U :
            elementIds.list.push_back ( elementIds.U );
            break;
        case displayMode :: V :
            elementIds.list.push_back ( elementIds.V );
            break;
        case displayMode :: UV :
            elementIds.list.push_back ( elementIds.UV );
            break;
    }

    switch ( earth_resources.globalMinMax ) {
        case true :
            if (earth_resources.mode != displayMode :: NONE)
                elementIds.list.push_back ( elementIds.global );
            break;
        case false :
            if (earth_resources.mode != displayMode :: NONE)
                elementIds.list.push_back ( elementIds.local );
            break;
    }

    renderUI (elementIds.list);
    elementIds.list.clear();

    renderProgressBar();

    glutSwapBuffers ();
}


int main (int argc, char ** argv) {
    glutInit(&argc, argv);
    // Parse command line params
    for ( int i = 1; i + 1 < argc; i+= 2 ) {
        if ( 
            strcmp ( "-f", argv[i]) == 0 ||
            strcmp ( "--file-path", argv[i]) == 0){

            commandLine.filePath = std::string( argv[i+1] );
        }
        else if ( 
            strcmp ( "-d", argv[i] ) == 0 ||
            strcmp ( "-size-divisor", argv[i]) == 0) {
            try {
                commandLine.divisor = std::stoi( argv[i+1]);
            }
            catch (std::invalid_argument const &e)
            {
                std::cerr << "Abort! Divisor input " << argv[i+1] << " invalid" << std::endl;
                return 1;
            }
            catch (std::out_of_range const &e)
            {
                std::cerr << "Abort! Divisor input " << argv[i+1] << " out of range" << std::endl;
                return 1;
            }
            if ( commandLine.divisor < 1 ) {
                std::cerr << "Abort! " << commandLine.divisor << " is an invalid size divisor" << std::endl;
                return 1;
            } 
            if ( commandLine.divisor > 20 ) {
                std::cerr << "Warning! " << commandLine.divisor << " too large for size divisor."  << std::endl;
            }
        }
        else {
            std::cerr << "Abort! Unkown commandline parameters: " << argv[i] << " " << argv[i+1] << std::endl;
            return 1;
        }
    }
    std::cout   << "Parsed Command Line Parameters " << std::endl
                << "  File Path: " << commandLine.filePath << std::endl 
                << "  Size Div.: " << commandLine.divisor << std::endl << std::flush;

    glutInitDisplayMode ( GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(800, 640);
    glutCreateWindow("Tsunami Viewer");
    glutDisplayFunc(&render);
    glutMouseFunc(&mouseButtonFunc);
    glutPassiveMotionFunc(&mousePassive);
    glutMotionFunc(&mouseActive);
    glutKeyboardFunc(&keyboardFunc);
    glutKeyboardUpFunc(&keyboardUpFunc);
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