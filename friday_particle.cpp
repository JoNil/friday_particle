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
	vec2 acc;
	float size = 0.1;
};

void updateParticles(Particle particle, int i, GLuint VBO) {
	GLfloat vertices[] = {
		-particle.size / 2 + particle.pos.x, -particle.size / 2 + particle.pos.y, -1.0f,
		particle.size / 2 + particle.pos.x, -particle.size / 2 + particle.pos.y, -1.0f,
		particle.size / 2 + particle.pos.x,  particle.size / 2 + particle.pos.y, -1.0f,
		-particle.size / 2 + particle.pos.x,  particle.size / 2 + particle.pos.y, -1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
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
		//RAND_MAX = 32767
		if (-0.55 < particles[i].pos.y && particles[i].pos.y < -0.45 && -0.05 < particles[i].pos.x && particles[i].pos.x < 0.05)
		{
			particles[i].speed.y = (float)(std::rand() % 1000 ) / 500.0f;
			particles[i].speed.x = (float)(std::rand() % 2000 - 1000) / 1500.0f;
			particles[i].acc.x = 0;
			particles[i].acc.y = 0;
		}
		else
		{
			particles[i].acc.x = -particles[i].pos.x;
			particles[i].acc.y = -(particles[i].pos.y + 0.5) * 2;
			if (particles[i].pos.y < -0.5)
			{
				particles[i].acc.y = -(particles[i].pos.y + 0.5) * 20;
				if (particles[i].acc.x < -0.05 && 0.05 < particles[i].acc.x)
					particles[i].acc.x = -particles[i].pos.x * 20;
			}
		}
		particles[i].speed += particles[i].acc * dt;
        particles[i].pos += particles[i].speed * dt;		
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
	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
    glViewport(0, 0, width, height);

    std::vector<Particle> particles(1000);

    for (int i = 0; i < (int)particles.size(); ++i) {
        particles[i].pos.x = 0;
        particles[i].pos.y = -0.5;
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
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);
    while (!glfwWindowShouldClose(window)) {

		oldTime = newTime;
		newTime = glfwGetTime();
		float deltaTime = newTime - oldTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simulate_particles(particles.data(), particles.size(), deltaTime);
		glBindVertexArray(VAO);
        for (int i = 0; i < (int)particles.size(); ++i) {
            updateParticles(particles[i], i, VBO);
			glDrawArrays(GL_QUADS, 0, 4);
			glBindVertexArray(0);
        }
		


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}