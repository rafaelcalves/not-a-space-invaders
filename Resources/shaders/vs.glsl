#version 410 core
layout(location=0) in vec2 vp;
layout(location=1) in vec2 vt;
uniform mat4 proj;
uniform mat4 matrix;
uniform float layerZ;
out vec2 texCoord;
void main () {
    texCoord = vt;
    gl_Position = proj * matrix * vec4(vp, layerZ, 1.0);
}