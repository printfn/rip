#include "UI.h"
#include "Direction.h"
#include "Pos.h"
#include "utils.h"
#include "VoxelPiece.h"
#include "Voxels.h"

#include "linmath.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

struct VertexData {
    float x, y, z;
    float r, g, b;
    float nx, ny, nz; // normal
    float dx, dy, dz; // direction of movement
    float movementStart;
};

struct Color {
    float r, g, b;
};

VertexData buildVertexData(float x, float y, float z, const VoxelPiece &piece) {
    return {
        x, y, z,
        piece.r, piece.g, piece.b,
        0, 0, 0,
        piece.dx, piece.dy, piece.dz,
        piece.movementStart,
    };
}

VertexData setNormal(VertexData data, Direction dir) {
    switch (dir) {
        case Direction::XP: data.nx = 1; data.ny = 0; data.nz = 0; break;
        case Direction::XN: data.nx = -1; data.ny = 0; data.nz = 0; break;
        case Direction::YP: data.nx = 0; data.ny = 1; data.nz = 0; break;
        case Direction::YN: data.nx = 0; data.ny = -1; data.nz = 0; break;
        case Direction::ZP: data.nx = 0; data.ny = 0; data.nz = 1; break;
        case Direction::ZN: data.nx = 0; data.ny = 0; data.nz = -1; break;
    }
    return data;
}

void addCube(float x, float y, float z, VoxelPiece piece, std::vector<VertexData> &data) {
    const std::vector<VertexData> vertices = {
        buildVertexData(x, y, z, piece),
        buildVertexData(x + 1, y, z, piece),
        buildVertexData(x, y + 1, z, piece),
        buildVertexData(x + 1, y + 1, z, piece),
        buildVertexData(x, y, z + 1, piece),
        buildVertexData(x + 1, y, z + 1, piece),
        buildVertexData(x, y + 1, z + 1, piece),
        buildVertexData(x + 1, y + 1, z + 1, piece),
    };
    const std::vector<int> indices = {
        0, 1, 2, 1, 2, 3, // front
        0, 2, 4, 2, 4, 6, // left
        1, 3, 5, 3, 5, 7, // right
        0, 1, 4, 1, 4, 5, // bottom
        2, 3, 6, 3, 6, 7, // top
        4, 5, 6, 5, 6, 7, // back
    };
    const std::vector<Direction> directions = {
        Direction::ZP,
        Direction::XN,
        Direction::XP,
        Direction::YN,
        Direction::YP,
        Direction::ZN,
    };
    for (int i = 0; i < (int)indices.size(); ++i) {
        int index = indices[i];
        Direction dir = directions[i / 6];
        data.push_back(setNormal(vertices[index], dir));
    }
}

void getVertexData(std::vector<VertexData> &vertexData, const Voxels &v, float time) {
    vertexData.clear();
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxY(); ++y) {
            for (int z = 0; z < v.maxZ(); ++z) {
                if (v.existsAt({x, y, z})) {
                    //int pieceIndex = (x + y + z) % 6;
                    int pieceIndex = v[{x, y, z}];
                    VoxelPiece properties = v.propertiesForPiece(pieceIndex);
                    addCube((float)x, (float)y, (float)z - time, properties, vertexData);
                }
            }
        }
    }
}

static const char* vertex_shader_text = R"(
uniform mat4 MVP;
uniform float fTime;
attribute vec3 vCol;
attribute vec3 vPos;
attribute vec3 vNormal;
attribute vec3 vMovement;
attribute float fMovementStart;
varying vec3 color;
varying vec3 normal;
varying vec3 fragPos;

void main() {
    float movementTime = max(0.0, fTime - fMovementStart);
    gl_Position = MVP * vec4(vPos + vMovement * movementTime, 1.0);
    color = vCol;
    normal = vNormal;
    fragPos = vPos + vMovement * movementTime;
}
)";
 
static const char* fragment_shader_text = R"(
varying vec3 color;
varying vec3 normal;
varying vec3 fragPos;
uniform vec3 vLightPos;

void main() {
    vec3 lightColor = vec3(1, 1, 1);

    // ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(vLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;
    gl_FragColor = vec4(result, 1.0);
}
)";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void error_callback(int error, const char *description) {
    std::cerr << "Error: " << description << " (error code " << error << ")" << std::endl;
}

void checkShader(GLuint shader) {
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled != GL_FALSE) {
        return;
    }
    std::cerr << "Failed to compile vertex shader:" << std::endl;

    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

    std::string errorMessage{errorLog.begin(), errorLog.end()};
    std::cerr << errorMessage << std::endl;

    exit(1);
}

int initGlfw(const Voxels &voxels) {
    // Set error callback, this can happen before GLFW initialisation
    glfwSetErrorCallback(error_callback);

    // Initialize the library
    if (!glfwInit())
        return -1;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(
        640, 480, "Recursive Interlocking Puzzles", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "GLFW Window creation failed" << std::endl;
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glClearColor(0.4f, 0.8f, 0.9f, 0.0f);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::vector<VertexData> vertexData;
    getVertexData(vertexData, voxels, 0.f);

    // Setup
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexData.size(),
        &vertexData[0], GL_STATIC_DRAW);
 
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    checkShader(vertex_shader);
 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    checkShader(fragment_shader);
 
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    GLint mvp_location = glGetUniformLocation(program, "MVP");
    GLint time_location = glGetUniformLocation(program, "fTime");
    GLint light_pos_location = glGetUniformLocation(program, "vLightPos");
    GLint vcol_location = glGetAttribLocation(program, "vCol");
    GLint vpos_location = glGetAttribLocation(program, "vPos");
    GLint vnormal_location = glGetAttribLocation(program, "vNormal");
    GLint vmovement_location = glGetAttribLocation(program, "vMovement");
    GLint fmovementstart_location = glGetAttribLocation(program, "fMovementStart");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), nullptr);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(vnormal_location);
    glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 6));
    glEnableVertexAttribArray(vmovement_location);
    glVertexAttribPointer(vmovement_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 9));
    glEnableVertexAttribArray(fmovementstart_location);
    glVertexAttribPointer(fmovementstart_location, 1, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 12));

    float cameraRotationHorizontal = 0;
    float cameraRotationVertical = 0;
    float cameraTime = 0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (cameraRotationVertical < deg2rad(90)) {
                cameraRotationVertical += deg2rad(1.5);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (cameraRotationVertical > deg2rad(-90)) {
                cameraRotationVertical -= deg2rad(1.5);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cameraRotationHorizontal += deg2rad(1.5);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cameraRotationHorizontal -= deg2rad(1.5);
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            // advance time forwards
            cameraTime -= 0.1;
            if (cameraTime < 0) {
                cameraTime = 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            // move time backwards
            cameraTime += 0.1;
        }

        // Rendering
        mat4x4 m, p, mvp;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        mat4x4_identity(m);

        // camera rotation
        mat4x4_translate_in_place(m, 0, 0, -15);

        mat4x4_rotate_X(m, m, cameraRotationVertical);
        mat4x4_rotate_Y(m, m, cameraRotationHorizontal);

        mat4x4_translate_in_place(m,
            -(float)voxels.maxX() / 2,
            -(float)voxels.maxY() / 2,
            -(float)voxels.maxZ() / 2);

        mat4x4_perspective(p, deg2rad(80), ratio, 1.f, -1.f);

        mat4x4_mul(mvp, p, m);
        
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
        glUniform1f(time_location, cameraTime);

        vec3 lightPos = {0, 0, -15};
        vec3_rotate_x(lightPos, cameraRotationVertical);
        vec3_rotate_y(lightPos, cameraRotationHorizontal);
        //std::cout << "light pos: (" << lightPos[0] << ", " << lightPos[1] << ", " << lightPos[2] << ")" << std::endl;
        glUniform3f(light_pos_location, lightPos[0], lightPos[1], lightPos[2]);

        glDrawArrays(GL_TRIANGLES, 0, vertexData.size());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 1;
}
