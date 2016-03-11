#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <vector>
#include <string>

using namespace glm;

const char * vertex_shader_source = ""
"layout(location = 0) in vec3 vertex_pos;"
""
"void main() {"
"    gl_Position = vec4(vertex_pos.xyz, 1.0f);"
"}"
"";

const char * fragment_shader_source = ""
"layout(location = 0) out vec4 color;"
""
"void main()"
"{"
"   color = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
"}"
"";

static void print_shader_compile_error(uint32_t shader)
{
    std::string infolog;

    int length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    infolog.resize((size_t)length);

    glGetShaderInfoLog(shader, length, nullptr, &infolog[0]);

    printf("%s", infolog.c_str());
}

struct Particle {
	vec2 pos;
	vec2 speed;
	float size = 0.1;
};

void draw_quad(Particle particle)
{
    glBegin(GL_QUADS);
    glVertex3f(-particle.size/2 + particle.pos.x, -particle.size/2 + particle.pos.y, -1.0f);
    glVertex3f( particle.size/2 + particle.pos.x, -particle.size/2 + particle.pos.y, -1.0f);
    glVertex3f( particle.size/2 + particle.pos.x,  particle.size/2 + particle.pos.y, -1.0f);
    glVertex3f(-particle.size/2 + particle.pos.x,  particle.size/2 + particle.pos.y, -1.0f);
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

    glewInit();

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

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);

    int status = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        print_shader_compile_error(vertex_shader);
    }
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        print_shader_compile_error(fragment_shader);
    }

    uint32_t shader_program = glCreateProgram();



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