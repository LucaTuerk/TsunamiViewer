#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "data/resources.h"
#include "data/structs.h"
#include "render/render.h"
#include "render/make.h"
#include "input/input.h"
#include "../res/res_defines.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>

static int makeResources (void) {
    camera.fov = 100.f;
    camera.distance = 5.f;

    camera.location = glm::vec3(0.,0.,5.);
    camera.theta = 0;

    if ( ! makeBackground ( NICE_SKY, STARS_TEX, VERT, FRAG ) )
        return 0;
    if ( ! makeEarth( 250, 250, EARTH_HEIGHT_TEX, EARTH_HEIGHT_TEX, STARS_TEX, STARS_TEX, EARTH_VERT, EARTH_FRAG) )
        return 0;
    return 1;
}

static void update (void) {
    // Fetch time
    int miliseconds = glutGet ( GLUT_ELAPSED_TIME );

    // Update aspect ratio
    camera.aspectRatio = (float) glutGet( GLUT_WINDOW_WIDTH ) / (float) glutGet( GLUT_WINDOW_HEIGHT );

    // Animate objects
    resources.fade_factor = sinf( (float) miliseconds * 0.000001f ) * 0.5f + 0.5f;

    // Update view
    updateCamera();

    glutPostRedisplay();
}

static void render (void) {
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    

    renderBackground ();
    renderEarth ();

    glutSwapBuffers ();
}


int main (int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(160, 90);
    glutCreateWindow("Tsunami Viewer");
    glutDisplayFunc(&render);
    glutMouseFunc(&mouseButtonFunc);
    glutPassiveMotionFunc(&mousePassive);
    glutMotionFunc(&mouseActive);
    glutIdleFunc(&update);

    glewInit();
    if(!GLEW_VERSION_2_0) {
        std::cerr << "OpenGL 2.0 not available." << std::endl;
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