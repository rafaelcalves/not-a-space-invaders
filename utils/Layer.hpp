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
