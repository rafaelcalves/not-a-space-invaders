//
//  Layer.hpp
//  OpenGL-BabySteps
//
//  Created by João Pedro Berton Palharini on 4/23/18.
//  Copyright © 2018 Unisinos. All rights reserved.
//
#include <stdio.h>

// GLFW
#include "Includes.h"

using namespace std;

class Layer {
public:
    float offsetX;
    float offsetY;
    float z;
    GLuint textures[8];

    
    Layer();
};
