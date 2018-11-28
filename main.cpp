#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"
#include "utils/Layer.hpp"
#include "utils/GameObject.hpp"
#include "utils/Collisions.h"
#include "utils/gl_utils.h"

GLuint compileFragmentShader(const char *shader);
GLuint compileVertexShader(const char *shader);
GLuint compileShader(const char *shader, GLuint resultShader);
void validateVertexShader(GLuint vs, GLint result, GLchar *infoLog);
void validateFragmentShader(GLuint fs, GLint result, GLchar *infoLog);
void validateProgram(GLuint program, GLint result, GLchar *infoLog);
void validateElement(GLuint element, GLint result, GLchar *infoLog, const GLchar *errorMessage);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int gameOver(char *reason);
int win();
GLuint prepareShaderProgram(GLuint vs, GLuint fs, GLint &result, GLchar *infoLog);
void attachShadersToShaderProgram(GLuint vs, GLuint fs, GLuint shaderProgram);
void prepareUniformForLayer(const GameObject *element, Layer *const *layers, int index);
void drawElement(const GameObject *element, Layer *const *layers, int index);
void bindSpaceship(Layer *const *layers);
void drawSpaceship(const GameObject *spaceship, Layer *const *layers);
void deleteGLObjects(const GameObject *element);
void resetsRocksPosition(GameObject *rock, float &lastPositionRock, bool &rockOnScreen, bool &projectileHit);
void setElementTexture(const GameObject *element, unsigned char *data, Layer *const *layers, int layerIndex, int &texWidth, int &texHeight, int &nrChannels);
void setSpaceshipTexture(const GameObject *spaceship, Layer *const *layers, int &texWidth, int &texHeight, int &nrChannels, unsigned char *&data);
void enableTestingAlpha();
void setImagesConfiguration();
void loadExplosionTexture(int &texWidth, int &texHeight, int &nrChannels, unsigned char *data, Layer *const *layers);

int WIDTH = 700, HEIGHT = 900;
int spaceshipXOffSet = 0;

bool projectileShot = false;

GLFWwindow* window;
glm::mat4 projection;

GLuint shaderProgram;

int main() {
    
    // Variables to hold texture properties
    int texWidth, texHeight, nrChannels;
    unsigned char* data;
    
	glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Prints the version of OpenGL
    const char* glfwversion = glfwGetVersionString();
    printf("GLFW %s\n", glfwversion);

    // Creates window with starting dimensions set in WIDTH and HEIGHT
    window = glfwCreateWindow(WIDTH, HEIGHT, "Gonzaga Invaders", nullptr, nullptr);
    
    // Get frameSpaceship buffer size to render viewport in proper scale - useful for HiDPI screens
    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    
    if (window == nullptr) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Sets up initial ortho and viewport
    projection =  glm::ortho(0.0f, (float) WIDTH, (float) HEIGHT, 0.0f, 0.1f, 100.0f);
    glViewport(0, 0, fbWidth, fbHeight);

    // Sets up texture processing
    glEnable(GL_TEXTURE_2D);

    enableTestingAlpha();
    setImagesConfiguration();

    // Background vertices
    GLfloat verticesBackground[] = {
        +0.0f,              +0.0f,      -0.1f, // top-left
        +0.0f,          (float) HEIGHT, -0.1f, // bottom-left
        (float) WIDTH,  (float) HEIGHT, -0.1f, // bottom-right
        (float) WIDTH,      +0.0f,      -0.1f  // top-right
    };
    
    GLfloat textureCoordsBackground[] = {
        +0.0f, +1.0f, // top-left
        +0.0f, +0.0f, // bottom-left
        +1.0f, +0.0f, // bottom-right
        +1.0f, +1.0f  // top-right
    };

    // Instantiating background
    auto background = new GameObject(verticesBackground, textureCoordsBackground, "../Resources/background.png");
    
    // Spaceship vertices
    GLfloat verticesSpaceship[] = {
        +0000.0f, +0775.0f, -0.1f, // top-left
        +0000.0f, +0875.0f, -0.1f, // bottom-left
        +0100.0f, +0875.0f, -0.1f, // bottom-right
        +0100.0f, +0775.0f, -0.1f  // top-right
    };

    // Setup for X = 0.125f initially because it's a sprite
    GLfloat textureCoordsSpaceship[] = {
        +0.000f, +1.000f, // top-left
        +0.000f, +0.000f, // bottom-left
        +0.125f, +0.000f, // bottom-right
        +0.125f, +1.000f  // top-right
    };

    // Instantiating spaceship
    auto spaceship = new GameObject(verticesSpaceship, textureCoordsSpaceship, "../Resources/spaceship-normal.png");

    // Rock vertices
    GLfloat verticesRock[] = {
            +0000.0f, -0100.0f, -0.1f, // top-left
            +0000.0f, +0000.0f, -0.1f, // bottom-left
            +0100.0f, +0000.0f, -0.1f, // bottom-right
            +0100.0f, -0100.0f, -0.1f  // top-right
    };

    GLfloat textureCoordsRock[] = {
            +0.0f, +1.0f, // top-left
            +0.0f, +0.0f, // bottom-left
            +1.0f, +0.0f, // bottom-right
            +1.0f, +1.0f  // top-right
    };

    // Instantiating rock
    auto rock = new GameObject(verticesRock, textureCoordsRock, "../Resources/gonzaga.png");

    // Projectile vertices
    GLfloat verticesProjectile[] = {
            +0046.0f, +0772.0f, -0.1f, // top-left
            +0046.0f, +0784.0f, -0.1f, // bottom-left
            +0054.0f, +0784.0f, -0.1f, // bottom-right
            +0054.0f, +0772.0f, -0.1f  // top-right
    };

    GLfloat textureCoordsProjectile[] = {
            +0.0f, +1.0f, // top-left
            +0.0f, +0.0f, // bottom-left
            +1.0f, +0.0f, // bottom-right
            +1.0f, +1.0f  // top-right
    };

    // Instantiating projectile
    auto projectile = new GameObject(verticesProjectile, textureCoordsProjectile, "../Resources/laser.png");

    // Generating 4 layers
    Layer* layers[4];
    
    layers[0] = new Layer;
    layers[1] = new Layer;
    layers[2] = new Layer;
    layers[3] = new Layer;
    
    layers[0]->z = -0.53f; // background
    layers[1]->z = -0.52f; // spaceship
    layers[2]->z = -0.51f; // rocks
    layers[3]->z = -0.50f; // projectiles
    
    // Activating current texture level
    glActiveTexture(GL_TEXTURE0);

    setElementTexture(background, data, layers, 0, texWidth, texHeight, nrChannels);
    setSpaceshipTexture(spaceship, layers, texWidth, texHeight, nrChannels, data);
    setElementTexture(rock, data, layers, 2, texWidth, texHeight, nrChannels);
    setElementTexture(projectile, data, layers, 3, texWidth, texHeight, nrChannels);

    background->load();
    spaceship->load();
    rock->load();
    projectile->load();

    GLint result;
    GLchar infoLog[512];

    char vertex_shader[1024 * 256];
    parse_file_into_str("../Resources/shaders/vs.glsl", vertex_shader, 1024 * 256);
    GLuint vs = compileVertexShader(vertex_shader);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    validateVertexShader(vs, result, infoLog);

    char fragment_shader[1024 * 256];
    parse_file_into_str("../Resources/shaders/fs.glsl", fragment_shader, 1024 * 256);
    GLuint fs = compileFragmentShader(fragment_shader);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    validateFragmentShader(fs,result,infoLog);

	shaderProgram = prepareShaderProgram(vs, fs, result, infoLog);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, background->translation);

    // Setting speed of movement for objects
    float speedSpaceship = 175.0f;
    float speedProjectile = -600.0f;
    float speedRock = 125.0f;

    // Initializes variables to load last position for translation of objects
    float lastPositionSpaceship = 0.0f;
    float lastPositionProjectile = 0.0f;
    float lastPositionRock = 0.0f;

    float frameSpaceship = 0.125f;
    bool rockOnScreen = false;
    bool spaceshipHit = false;
    bool projectileHit = false;
    int rocksDestroyed = 0;

    // Allow some time for the player to read instructions
    usleep(3000000);

	while (!glfwWindowShouldClose(window)) {

        // Sets mouse click callback
        glfwSetKeyCallback(window, key_callback);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawElement(background, layers, 0);
        drawSpaceship(spaceship, layers);
        drawElement(rock, layers, 2);
        drawElement(projectile, layers, 3);

        // Animating the spaceship's sprite
        if (!spaceshipHit) {
            if (layers[1]->offsetX == 0.875f) {
                layers[1]->offsetX = 0.0f;
            }
            layers[1]->offsetX = layers[1]->offsetX + frameSpaceship;
        } else {
            if (layers[1]->offsetX == 0.875f) {
                return gameOver("The spaceship was hit!");
            }
            layers[1]->offsetX = layers[1]->offsetX + frameSpaceship;
        }

        glUniform1f(glGetUniformLocation(shaderProgram, "offsetX"), layers[1]->offsetX);

        // Moving the spaceship and projectile while it's not shot
        static double previousSecondsTrans = glfwGetTime();
        double currentSecondsTrans = glfwGetTime();
        double elapsedSecondsTrans = currentSecondsTrans - previousSecondsTrans;
        previousSecondsTrans = currentSecondsTrans;

        if (lastPositionSpaceship > WIDTH-100 || lastPositionSpaceship < 0){
            speedSpaceship = -speedSpaceship;
        }

        spaceship->translation[12] = elapsedSecondsTrans * speedSpaceship + lastPositionSpaceship;
        if (!projectileShot) {
            projectile->translation[12] = elapsedSecondsTrans * speedSpaceship + lastPositionSpaceship;
        } else {
            // If projectile is shot, move it on Y axis until it hits a rock or exits the screen
            if (projectile->translation[13] >= -HEIGHT && !projectileHit) {
                projectile->translation[13] = elapsedSecondsTrans * speedProjectile + lastPositionProjectile;
                lastPositionProjectile = projectile->translation[13];
            } else {
                projectile->translation[13] = 0.0f;
                lastPositionProjectile = projectile->translation[13];
                projectileShot = false;
                projectileHit = false;
            }
        }

        lastPositionSpaceship = spaceshipXOffSet;

        // Setting the rock's initial position randomly if drawing a new one
        if (!rockOnScreen) {
            rock->translation[12] = rand() % (WIDTH - 125) + 25;
        }

        // Moving the rock until it hits the bottom
        if (rock->translation[13] <= HEIGHT) {
            rock->translation[13] = elapsedSecondsTrans * speedRock + lastPositionRock;
            lastPositionRock = rock->translation[13];
            rockOnScreen = true;
        } else {
            // Rock went past screen, game over
            return gameOver("A rock hit earth!");
        }

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, spaceship->translation);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, projectile->translation);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, rock->translation);

        // Check collisions
        if (rockOnSpaceship(rock, spaceship)) {
            loadExplosionTexture(texWidth, texHeight, nrChannels, data, layers);
            spaceshipHit = true;
        }

        // If projectile has been shot and hits rock
        if (projectileShot && projectileOnRock(projectile, rock)) {
            // Resets the positioning of the rock
            resetsRocksPosition(rock, lastPositionRock, rockOnScreen, projectileHit);
            // Increase count of destroyed rocks
            rocksDestroyed = rocksDestroyed + 1;
        }

        // If the player hit 10 rocks, he wins
        if (rocksDestroyed == 10) {
            return win();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deleteGLObjects(spaceship);
    deleteGLObjects(background);
    deleteGLObjects(rock);
    deleteGLObjects(projectile);

    glfwTerminate();

	return EXIT_SUCCESS;
}

void loadExplosionTexture(int &texWidth, int &texHeight, int &nrChannels, unsigned char *data, Layer *const *layers) {
    data = stbi_load("../Resources/spaceship-explosion.png", &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, layers[1]->textures);

    for (int i = 0; i < 8; i = i + 1) {
                glBindTexture(GL_TEXTURE_2D, layers[1]->textures[i]);
            }

    if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                cout << "Failed to load texture for spaceship explosion" << endl;
            }

    stbi_image_free(data);
}

void setImagesConfiguration() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void enableTestingAlpha() {
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
setSpaceshipTexture(const GameObject *spaceship, Layer *const *layers, int &texWidth, int &texHeight, int &nrChannels,
                    unsigned char *&data) {
    data = stbi_load(spaceship->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, layers[1]->textures);

    for (int i = 0; i < 8; i = i + 1) {
        glBindTexture(GL_TEXTURE_2D, layers[1]->textures[i]);
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "Failed to load texture for spaceship" << endl;
    }

    stbi_image_free(data);
}

void setElementTexture(const GameObject *element, unsigned char *data, Layer *const *layers, int layerIndex, int &texWidth, int &texHeight, int &nrChannels) {
    data = stbi_load(element->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, layers[layerIndex]->textures);

    glBindTexture(GL_TEXTURE_2D, layers[layerIndex]->textures[0]);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "Failed to load texture for background" << endl;
    }

    stbi_image_free(data);
}

void resetsRocksPosition(GameObject *rock, float &lastPositionRock, bool &rockOnScreen, bool &projectileHit) {
    rock->translation[13] = 0.0f;
    lastPositionRock = 0.0f;
    rockOnScreen = false;
    projectileHit = true;
}

void deleteGLObjects(const GameObject *element) {
    glDeleteVertexArrays(1, &element->vao);
    glDeleteBuffers(1, &element->vbo);
    glDeleteBuffers(1, &element->vboTexture);
    glDeleteBuffers(1, &element->ebo);
}

void drawSpaceship(const GameObject *spaceship, Layer *const *layers) {
    glBindVertexArray(spaceship->vao);

    prepareUniformForLayer(spaceship, layers, 1);

    glActiveTexture(GL_TEXTURE0);

    bindSpaceship(layers);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void bindSpaceship(Layer *const *layers) {
    for (int i = 0; i < 8; i = i + 1) {
        glBindTexture(GL_TEXTURE_2D, layers[1]->textures[i]);
    }
}

void drawElement(const GameObject *element, Layer *const *layers, int index) {
    prepareUniformForLayer(element, layers, index);
    glBindVertexArray(element->vao);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, layers[index]->textures[0]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void prepareUniformForLayer(const GameObject *element, Layer *const *layers, int index) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, element->translation);
    glUniform1f(glGetUniformLocation(shaderProgram, "offsetX"), layers[index]->offsetX);
    glUniform1f(glGetUniformLocation(shaderProgram, "layerZ"), layers[index]->z);
    glUniform1f(glGetUniformLocation(shaderProgram, "theTexture"), 0);
}

GLuint compileFragmentShader(const char *shader) {
    GLuint resultShader = glCreateShader(GL_FRAGMENT_SHADER);
    return compileShader(shader, resultShader);
}

GLuint compileVertexShader(const char *shader) {
    GLuint resultShader = glCreateShader(GL_VERTEX_SHADER);
    return compileShader(shader, resultShader);
}

GLuint compileShader(const char *shader, GLuint resultShader) {
    const GLchar *p = shader;
    glShaderSource(resultShader, 1, &p, NULL);
    glCompileShader(resultShader);
    return resultShader;
}

void validateVertexShader(GLuint vs, GLint result, GLchar *infoLog) {
    const GLchar *vertexShaderErrorMessage = "Error! Vertex shader failed to compile. ";
    validateElement(vs, result, infoLog, vertexShaderErrorMessage);
}

void validateFragmentShader(GLuint fs, GLint result, GLchar *infoLog) {
    const GLchar *fragmentShaderErrorMessage = "Error! Fragment shader failed to compile. ";
    validateElement(fs, result, infoLog, fragmentShaderErrorMessage);
}

void validateProgram(GLuint program, GLint result, GLchar *infoLog) {
    const GLchar *programErrorMessage = "Error! Program failed to compile. ";
    validateElement(program, result, infoLog, programErrorMessage);
}

void validateElement(GLuint element, GLint result, GLchar *infoLog, const GLchar *errorMessage) {
    if (!result) {
        glGetShaderInfoLog(element, sizeof(infoLog), NULL, infoLog);
        std::cout << errorMessage << infoLog << std::endl;
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    switch (key){
        case GLFW_KEY_LEFT:
            if(spaceshipXOffSet >= 0) {
                spaceshipXOffSet = spaceshipXOffSet - 10;
            }
            break;
        case GLFW_KEY_RIGHT:
            if(spaceshipXOffSet <= WIDTH - 100) {
                spaceshipXOffSet = spaceshipXOffSet + 10;
            }
            break;
        case GLFW_KEY_SPACE:
            if(action == GLFW_RELEASE) projectileShot = true;
            break;
        default:
            break;
    }
}

int gameOver(char *reason) {
    std::cout << "Game Over! " << reason << std::endl;
    usleep(2000000);
    return EXIT_SUCCESS;
}

int win() {
    std::cout << "You won! Congratulations!" << std::endl;
    usleep(2000000);
    return EXIT_SUCCESS;
}

GLuint prepareShaderProgram(GLuint vs, GLuint fs, GLint &result, GLchar *infoLog) {
    GLuint shaderProgram = glCreateProgram();
    attachShadersToShaderProgram(vs, fs, shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    validateProgram(shaderProgram,result,infoLog);
    return shaderProgram;
}

void attachShadersToShaderProgram(GLuint vs, GLuint fs, GLuint shaderProgram) {
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
}
