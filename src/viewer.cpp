#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

static int make_resources (void) {
    return 1;
}

static void update (void) {

}

static void render (void) {
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}


int main (int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(400, 300);
    glutCreateWindow("Tsunami Viewer");
    glutDisplayFunc(&render);
    glutIdleFunc(&update);

    glewInit();
    if(!GLEW_VERSION_2_0) {
        std::cerr << "OpenGL 2.0 not available." << std::endl;
        return 1;
    }

    if(!make_resources()) {
        std::cerr << "Failed to load resources." << std::endl;
    }

    glutMainLoop();
    return 0;
}