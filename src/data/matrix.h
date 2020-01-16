#ifndef _MATRIX_H
#define _MATRIX_H
#define GLM_ENABLE_EXPERIMENTAL 

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "resources.h"
#include "structs.h"
#include "global_defines.h"

glm::mat4 cameraModel () {
    return 
        glm::translate( camera.location );
        //* 
        //glm::lookAt (
        //    camera.location,
        //    earth_resources.location,
        //    glm::vec3(1,0,0)
        //);
}

glm::mat4 view () {
    return glm::inverse ( cameraModel() );
}

glm::mat4 projection () {

    return 
        glm::perspective( camera.fov, camera.aspectRatio, 0.1f, 100.f );
}

glm::mat4 earth_model () {
    return
    glm::translate (
        glm::vec3 (
            0.,
            0.,
            - camera.distance
        )
    ) *
    glm::toMat4( 
        glm::angleAxis( camera.theta, glm::vec3 (1,0,0)) *
        glm::angleAxis( camera.lambda, glm::vec3 (0,1,0))
     ) *
    glm::scale ( earth_resources.scale );
}

glm::vec3 viewDir () {
    glm::vec4 view = cameraModel() * glm::vec4 (0.f,0.f,1.f, 1.f);
    return glm::vec3 (view.x, view.y, view.z);
}

#endif
