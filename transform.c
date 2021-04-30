#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "cglm/affine.h"
#include "common.h"
#include "stb_image.h"

static const float vertices[] =
{
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
};

static const unsigned indices[] = { 0, 1, 3, 1, 2, 3 };

static const char* vertex_shader =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec2 aTexCoord;\n"
	"uniform mat4 cTransform;\n"
	"out vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"    texCoord = aTexCoord;\n"
	"    gl_Position = cTransform * vec4(aPos, 1.0);\n"
	"}";

static const char* fragment_shader =
	"#version 330 core\n"
	"uniform sampler2D sTexture;\n"
	"in vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = texture(sTexture, texCoord);\n"
	"}";

int main(int argc, char** argv)
{
	// =====================================
	// Initialisation
	// =====================================
	// SDL

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		error("SDL Error", SDL_GetError());
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_Window* window = SDL_CreateWindow("OpenGL Tutorial 01",
										 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										 1024, 768, SDL_WINDOW_OPENGL);
	if (!window)
	{
		error("SDL Error", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
	{
		error("SDL Error", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		error("GLEW Error", glewGetErrorString(glGetError()));
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// STB Image
	stbi_set_flip_vertically_on_load(1);

	// Vertex Buffers
	unsigned vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	if (!validate_gl("VAO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	if (!validate_gl("VBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	if (!validate_gl("EBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Shader
	int success;

	const unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, (const char* const*)&vertex_shader, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(vertex, ERROR_BUFFER_SIZE, NULL, message);
		error("Vertex Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	const unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, (const char* const*)&fragment_shader, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(fragment, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	const unsigned program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetProgramInfoLog(program, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Texture
	unsigned texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;
	unsigned char* texture_data = stbi_load("data/textures/crate_diffuse.png", &width, &height, &channels, 0);
	if (!texture_data)
	{
		error("Texture Loading Error", "Could not found file data/textures/crate_diffuse.png.");
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_data);
	if (!validate_gl("VAO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Shader Uniforms
	const unsigned uniform_transform = glGetUniformLocation(program, "cTransform");

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "sTexture"), 0);
	glUseProgram(0);

	// =====================================
	// Rendering
	// =====================================
	vec3 move = { 0.25f, -0.25f, 0.0f };
	vec3 scale = { 0.5f, 0.5f, 0.5f };
	vec3 distance = { 1.0f, 0.0f, 0.0f };
	mat4 transform;

	SDL_Event event;
	float ticks;
	int run = 1;
	while (run)
	{
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				run = 0;

		ticks = (float)(SDL_GetTicks()) * 0.001f;
		glm_mat4_identity(transform);
		glm_scale(transform, scale);
		glm_rotate_z(transform, -ticks, transform);
		glm_translate(transform, distance);
		glm_rotate_z(transform, ticks * 2.0f, transform);

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
		glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, transform[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(float), GL_UNSIGNED_INT, 0);

		if (validate_gl("Open GL Rendering Error"))
			SDL_GL_SwapWindow(window);
		else
			run = 0;
	}

	// =====================================
	// Destruction
	// =====================================
	// Texture
	glDeleteTextures(1, &texture);

	// Shader
	glDeleteProgram(program);

	// Vertex Buffers
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	// SDL
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

__declspec(dllexport) unsigned NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
