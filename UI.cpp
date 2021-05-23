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
};

struct Color {
    float r, g, b;
};

void addCube(float x, float y, float z, Color color, std::vector<VertexData> &data) {
    float r = color.r;
    float g = color.g;
    float b = color.b;
    std::vector<VertexData> vertices = {
        { x, y, z, r, g, b },
        { x + 1, y, z, r, g, b },
        { x, y + 1, z, r, g, b },
        { x + 1, y + 1, z, r, g, b },
        { x, y, z + 1, r, g, b },
        { x + 1, y, z + 1, r, g, b },
        { x, y + 1, z + 1, r, g, b },
        { x + 1, y + 1, z + 1, r, g, b },
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

std::vector<VertexData> getVertexData(const Voxels &v) {
    std::vector<VertexData> vertexData = {
        { -0.6f, -0.4f, 0.f, 1.f, 0.f, 0.f },
        {  0.6f, -0.4f, 0.f, 0.f, 1.f, 0.f },
        {   0.f,  0.6f, 0.f, 0.f, 0.f, 1.f }
    };
    vertexData = {};
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxY(); ++y) {
            for (int z = 0; z < v.maxZ(); ++z) {
                if (v.existsAt({x, y, z})) {
                    Color color = {1, 0, 0};
                    if ((x + y + z) % 3 == 1) {
                        color = {0, 1, 0};
                    } else if ((x + y + z) % 3 == 2) {
                        color = {0, 0, 1};
                    }
                    addCube((float)x, (float)y, (float)z, color, vertexData);
                }
            }
        }
    }

    return vertexData;
}

static const char* vertex_shader_text = R"(
#version 110
uniform mat4 MVP;
attribute vec3 vCol;
attribute vec3 vPos;
varying vec3 color;

void main() {
    gl_Position = MVP * vec4(vPos, 1.0);
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
    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto vertexData = getVertexData(voxels);

    // Setup
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
 
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
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), nullptr);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexData), (void *)(sizeof(float) * 3));

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        mat4x4 m, p, mvp;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        mat4x4_identity(m);
        mat4x4_translate_in_place(m, 1, 0, -30);
        mat4x4_rotate_X(m, m, (float) glfwGetTime());
        mat4x4_perspective(p, deg2rad(80), ratio, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
 
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
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
