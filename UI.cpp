#include "UI.h"
#include "utils.h"
#include "Voxels.h"
#include "Pos.h"

#include "linmath.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

struct VertexData {
    float x, y, z;
    float r, g, b;
    float dx, dy, dz; // direction of movement
};

struct Color {
    float r, g, b;
};

void addCube(float x, float y, float z, Color color, std::vector<VertexData> &data) {
    float r = color.r;
    float g = color.g;
    float b = color.b;
    std::vector<VertexData> vertices = {
        { x, y, z, r, g, b, 1, 0, 0 },
        { x + 1, y, z, r, g, b, 1, 0, 0 },
        { x, y + 1, z, r, g, b, 1, 0, 0 },
        { x + 1, y + 1, z, r, g, b, 1, 0, 0 },
        { x, y, z + 1, r, g, b, 1, 0, 0 },
        { x + 1, y, z + 1, r, g, b, 1, 0, 0 },
        { x, y + 1, z + 1, r, g, b, 1, 0, 0 },
        { x + 1, y + 1, z + 1, r, g, b, 1, 0, 0 },
    };
    std::vector<int> indices = {
        0, 1, 2, 1, 2, 3, // front
        0, 2, 4, 2, 4, 6, // left
        1, 3, 5, 3, 5, 7, // right
        0, 1, 4, 1, 4, 5, // bottom
        2, 3, 6, 3, 6, 7, // top
        4, 5, 6, 5, 6, 7, // back
    };
    for (int i : indices) {
        data.push_back(vertices[i]);
    }
}

Color colorForIndex(int i) {
    switch (i) {
        case 0:
            return {1, 0, 0};
        case 1:
            return {0, 1, 0};
        case 2:
            return {0, 0, 1};
        case 3:
            return {1, 1, 0};
        case 4:
            return {1, 0, 1};
        default:
            return {0, 1, 1};
    }
}

void getVertexData(std::vector<VertexData> &vertexData, const Voxels &v, float time) {
    vertexData.clear();
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxY(); ++y) {
            for (int z = 0; z < v.maxZ(); ++z) {
                if (v.existsAt({x, y, z})) {
                    Color color = colorForIndex((x + y + z) % 6);
                    //color = colorForIndex(v[{x, y, z}]);
                    addCube((float)x, (float)y, (float)z - time, color, vertexData);
                }
            }
        }
    }
}

static const char* vertex_shader_text = R"(
#version 110
uniform mat4 MVP;
uniform float fTime;
attribute vec3 vCol;
attribute vec3 vPos;
attribute vec3 vMovement;
varying vec3 color;

void main() {
    gl_Position = MVP * vec4(vPos + vMovement * fTime, 1.0);
    color = vCol;
}
)";
 
static const char* fragment_shader_text = R"(
#version 110
varying vec3 color;

void main() {
    gl_FragColor = vec4(color, 1.0);
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

int initGlfw(const Voxels &voxels) {
    // Set error callback, this can happen before GLFW initialisation
    glfwSetErrorCallback(error_callback);

    // Initialize the library
    if (!glfwInit())
        return -1;

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
 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    GLint mvp_location = glGetUniformLocation(program, "MVP");
    GLint vpos_location = glGetAttribLocation(program, "vPos");
    GLint vcol_location = glGetAttribLocation(program, "vCol");
    GLint vmovement_location = glGetAttribLocation(program, "vMovement");
    GLint time_location = glGetUniformLocation(program, "fTime");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), nullptr);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(vmovement_location);
    glVertexAttribPointer(vmovement_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 6));

    float cameraRotationHorizontal = 0;
    float cameraRotationVertical = 0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (cameraRotationVertical < deg2rad(90)) {
                cameraRotationVertical += deg2rad(1);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (cameraRotationVertical > deg2rad(-90)) {
                cameraRotationVertical -= deg2rad(1);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cameraRotationHorizontal += deg2rad(1);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cameraRotationHorizontal -= deg2rad(1);
        }

        // Rendering
        mat4x4 m, p, mvp;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        mat4x4_identity(m);
        mat4x4_translate_in_place(m, -(float)voxels.maxX() / 2, -(float)voxels.maxY() / 2, -(float)voxels.maxZ() / 2);
        mat4x4_translate_in_place(m, 0, 0, -15);

        // camera rotation
        mat4x4_rotate_X(m, m, cameraRotationVertical);
        mat4x4_rotate_Y(m, m, cameraRotationHorizontal);
        mat4x4_perspective(p, deg2rad(80), ratio, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
 
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
        //glUniform1f(time_location, glfwGetTime());
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
