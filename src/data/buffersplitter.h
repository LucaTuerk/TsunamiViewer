#ifndef _BUFFERSPLITTER_H
#define _BUFFERSPLITTER_H

#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <vector>
#include "structs.h"
#include <map>
#include <climits>


static void splitBuffers ( std::vector<vertex> & verticies, std::vector<GLuint> & indicies, std::vector< std::vector< vertex > > & vertVector, std::vector< std::vector < GLushort > > & indVector) {
    
    std::map < GLuint, GLushort > map;
    GLushort index = 0;
    int vertexBuffer = 0;
    int indexBuffer = 0;
    vertVector.emplace_back();
    indVector.emplace_back();

    for ( int i = 0; i < indicies.size(); i++ ) {
        if ( index > 50000 && i % 3 == 0) {
            index = 0;
            vertVector.emplace_back();
            indVector.emplace_back();
            vertexBuffer ++;
            indexBuffer ++;
            map.clear();
            // Offset to overlap tris to avoid gaps
            i -= 3000;
        }
        if ( map.find ( indicies[i] ) == map.end () ) {
            map[ indicies[i] ] = index ++;
            vertVector[vertexBuffer].push_back ( verticies[ indicies[i] ] );
        }

        indVector [ indexBuffer ] .push_back ( map[ indicies[ i ] ] );
    }
}

#endif