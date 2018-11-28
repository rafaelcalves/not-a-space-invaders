//
//  Spaceship.cpp
//  OpenGL-BabySteps
//
//  Created by João Pedro Berton Palharini on 4/23/18.
//  Copyright © 2018 Unisinos. All rights reserved.
//

#ifndef GameObject_H
#define GameObject_H

#include "Includes.h"

using namespace std;

class GameObject {
public:
    // Attributes
    GLfloat vertices[16];
    GLfloat translation[16];
    GLfloat textureCoord[8];
    unsigned int indices[6];
    GLuint vao, vbo, vboTexture, ebo;
    const char *textureFile;
    
    // Methods
    GameObject(GLfloat* v, GLfloat* t, const char *filename);
    void load();
    void draw();
};
#endif
