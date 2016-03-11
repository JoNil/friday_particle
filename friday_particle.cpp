#include <cstdio>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

using namespace glm;

void draw_quad() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.75f, -0.75f, -1.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( 0.75f, -0.75f, -1.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 0.75f,  0.75f, -1.0f);
        glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(-0.75f,  0.75f, -1.0f);
    glEnd();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

struct Particle {
    vec2 pos;
    vec2 speed;
};

void simulate_particles(Particle * particles, int particle_count, float dt)
{
    for (int i = 0; i < particle_count; ++i) {
        particles[i].pos += particles[i].speed * dt;
        particles[i].speed += particles[i].pos * dt;
    }
}

int main(int argc, char ** argv)
{
    int width = 640;
    int height = 480;

    glfwInit();

    GLFWwindow * window = glfwCreateWindow(width, height, "Friday particle", NULL, NULL);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    while (!glfwWindowShouldClose(window)) {
        draw_quad();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}