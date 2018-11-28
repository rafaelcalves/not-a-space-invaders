#include "GameObject.hpp"

GameObject::GameObject(GLfloat* v, GLfloat* t, const char *filename) {
    for (int i = 0; i < 16; i = i + 1) {
        vertices[i] = v[i];
    }
    for (int i = 0; i < 8; i = i + 1) {
        textureCoord[i] = t[i];
    }
    
    // Populates translation matrix as identity
    for (int i = 0; i < 16; i = i + 1) {
        if (i % 5 == 0) {
            translation[i] = 1.0f;
        } else {
            translation[i] = 0.0f;
        }
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    textureFile = filename;
}

void GameObject::load() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Texture buffer & pointer
    glGenBuffers(1, &vboTexture);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoord), textureCoord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void GameObject::draw() {

}