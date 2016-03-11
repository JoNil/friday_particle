#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <vector>
#include <string>

using namespace glm;

void check_gl_error(const char * stmt, const char * fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        abort();
    }
}

#define GL(stmt) do { \
    stmt; \
    check_gl_error(#stmt, __FILE__, __LINE__); \
} while (0)

#define GLSL(version, shader) "#version " #version "\n" #shader

const char * vertex_shader_source = GLSL(130,

attribute vec3 vertex_pos;

varying vec3 pos;

void main() {
    pos = vertex_pos;
    gl_Position = vec4(vertex_pos, 1.0);
});

const char * fragment_shader_source = GLSL(130,

varying vec3 pos;

void main()
{
   gl_FragColor = vec4(1.0, 2.0 * pos.x + 1.0, 2.0 * pos.y + 1.0, 1.0);
});

static void print_shader_compile_error(uint32_t shader)
{
    std::string infolog;

    int length = 0;
    GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));

    infolog.resize((size_t)length);

    GL(glGetShaderInfoLog(shader, length, nullptr, &infolog[0]));

    printf("%s\n", infolog.c_str());
}

struct Particle {
	vec2 pos;
	vec2 speed;
	float size = 0.1;
};

void updateParticles(Particle particle, GLfloat *vertices,  int offset) {
	vertices[offset] = -particle.size / 2 + particle.pos.x;
	vertices[offset + 1] = -particle.size / 2 + particle.pos.y; 
	vertices[offset + 2] = -1.0f;

	vertices[offset + 3] = particle.size / 2 + particle.pos.x;
	vertices[offset + 4] = -particle.size / 2 + particle.pos.y;
	vertices[offset + 5] = -1.0f;

	vertices[offset + 6] = particle.size / 2 + particle.pos.x;
	vertices[offset + 7] = particle.size / 2 + particle.pos.y;
	vertices[offset + 8] = -1.0f;

	vertices[offset + 9] = -particle.size / 2 + particle.pos.x;
	vertices[offset + 10] = particle.size / 2 + particle.pos.y;
	vertices[offset + 11] = - 1.0f;
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
    int width = 1024;
    int height = 768;

    std::srand(std::time(0));

    glfwInit();

    GLFWwindow * window = glfwCreateWindow(width, height, "Friday particle", NULL, NULL);
	glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	const int numParticles = 1000;
    std::vector<Particle> particles(numParticles);

    for (int i = 0; i < (int)particles.size(); ++i) {
        particles[i].pos.x = (float)(std::rand() % 1000 - 500) / 500.0f;
        particles[i].pos.y = (float)(std::rand() % 1000 - 500) / 500.0f;
    }

    GL(glViewport(0, 0, width, height));
    GL(glClearColor(0.0f, 0.0f, 0.2f, 1.0f));
    GL(glDisable(GL_DEPTH_TEST));

    int status = 0;
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GL(glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr));
    GL(glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr));

    GL(glCompileShader(vertex_shader));
    GL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Vertex shader error:\n");
        print_shader_compile_error(vertex_shader);
        exit(1);
    }

    GL(glCompileShader(fragment_shader));
    GL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Fragment shader error:\n");
        print_shader_compile_error(fragment_shader);
        exit(1);
    }

    uint32_t shader_program = glCreateProgram();
    GL(glAttachShader(shader_program, vertex_shader));
    GL(glAttachShader(shader_program, fragment_shader));
    GL(glLinkProgram(shader_program));

    GL(glValidateProgram(shader_program));
    GL(glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Link error\n");
        print_shader_compile_error(shader_program);
        exit(1);
    }

    GL(glUseProgram(shader_program));

	float oldTime = 0;
	float newTime = glfwGetTime();

	GLuint VBO;
	GLfloat particleVertices[numParticles * 12];
	GL(glGenBuffers(1, &VBO));
	GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));

    while (!glfwWindowShouldClose(window)) {

		oldTime = newTime;
		newTime = glfwGetTime();
		float deltaTime = newTime - oldTime;

        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        simulate_particles(particles.data(), particles.size(), deltaTime);


        for (int i = 0; i < (int)particles.size(); ++i) {
            updateParticles(particles[i], particleVertices, i*12);	
        }
		GL(glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices, GL_STATIC_DRAW));
		GL(glDrawArrays(GL_QUADS, 0, numParticles*4));


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}