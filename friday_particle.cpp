#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace glm;

const char * vertex_shader = "
layout(location = 0) in vec3 vertex_pos;

void main() {    
    gl_Position = vec4(vertex_pos.xyz, 1.0f);
}
";

const char * fragment_shader = "

layout(location = 0) out vec4 color;

uniform sampler2D tex;
uniform float alphaFade;
uniform float colorFade;

void main()
{
    vec4 texColor = texture(tex, frag_uv);

    color = vec4(colorFade * texColor.xyz, alphaFade * texColor.a);
}
";

struct Particle {
	vec2 pos;
	vec2 speed;
	float size = 0.1;
};

void draw_quad(Particle particle)
{
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-particle.size/2 + particle.pos.x, -particle.size/2 + particle.pos.y, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( particle.size/2 + particle.pos.x, -particle.size/2 + particle.pos.y, -1.0f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( particle.size/2 + particle.pos.x,  particle.size/2 + particle.pos.y, -1.0f);
    glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(-particle.size/2 + particle.pos.x,  particle.size/2 + particle.pos.y, -1.0f);
    glEnd();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void simulate_particles(Particle * particles, int particle_count, float dt)
{
	
    for (int i = 0; i < particle_count; ++i) {

        float random_x = (float)(std::rand() % 1000 - 500) / 40000.0f;
        float random_y = (float)(std::rand() % 1000 - 500) / 40000.0f;

        particles[i].pos += particles[i].speed * dt;
        particles[i].speed -= particles[i].pos * dt;
        particles[i].speed += vec2(random_x, random_y);
    }
}

int main(int argc, char ** argv)
{
    int width = 640;
    int height = 480;

    std::srand(std::time(0));

    glfwInit();

    GLFWwindow * window = glfwCreateWindow(width, height, "Friday particle", NULL, NULL);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    std::vector<Particle> particles(1000);

    for (int i = 0; i < (int)particles.size(); ++i) {
        particles[i].pos.x = (float)(std::rand() % 1000 - 500) / 500.0f;
        particles[i].pos.y = (float)(std::rand() % 1000 - 500) / 500.0f;
    }

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	float oldTime = 0;
	float newTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {

		oldTime = newTime;
		newTime = glfwGetTime();
		float deltaTime = newTime - oldTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simulate_particles(particles.data(), particles.size(), deltaTime);

        for (int i = 0; i < (int)particles.size(); ++i) {
            draw_quad(particles[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}