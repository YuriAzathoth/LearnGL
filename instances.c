#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "cglm/affine.h"
#include "cglm/cam.h"
#include "cglm/quat.h"
#include "stb_image.h"

#define ERROR_BUFFER_SIZE 2048
#define FILENAME_BUFFER_SIZE 256

static void error(const char* title, const char* format, ...);
static int validate_gl(const char* title);
static int load_shaders_text(char** vertex_shader, char** fragment_shader, const char* filename);

static const float vertices[] =
{
	 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		//  0 Right Up Back
	 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		//  1 Right Down Back
	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		//  2 Left Down Back
	-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		//  3 Left Up Back
	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f,		//  4 Left Up Forward
	-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		//  5 Left Down Forward
	 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,		//  6 Right Down Forward
	 0.5f,  0.5f, -0.5f,	1.0f, 0.0f,		//  7 Right Up Forward
	 0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		//  8 (0) Right Up Back
	-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		//  9 (3) Left Up Back
	-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		// 10 (4) Left Up Forward
	 0.5f,  0.5f, -0.5f,	0.0f, 1.0f		// 11 (7) Right Up Forward
};

static const unsigned indices[] =
{
	0, 1, 3, 1, 2, 3,	// Front
	6, 7, 5, 7, 4, 5,	// Back
	7, 0, 4, 0, 3, 4,	// Top
	1, 6, 2, 6, 5, 2,	// Bottom
	2, 5, 9, 5, 10, 9,	// Left
	8, 11, 1, 11, 6, 1	// Right
};

static vec3 positions[] =
{
	{  0.0f,  0.0f,  0.0f },
	{  2.0f,  5.0f, -15.0f },
	{ -1.5f, -2.2f, -2.5f },
	{ -3.8f, -2.0f, -12.3f },
	{  2.4f, -0.4f, -3.5f },
	{ -1.7f,  3.0f, -7.5f },
	{  1.3f, -2.0f, -2.5f },
	{  1.5f,  2.0f, -2.5f },
	{  1.5f,  0.2f, -1.5f },
	{ -1.3f,  1.0f, -1.5f }
};

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

	// OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	char* vertex_shader = NULL;
	char* fragment_shader = NULL;
	if (!load_shaders_text(&vertex_shader, &fragment_shader, "data/shaders/4_cube"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

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
	unsigned char* texture_data = stbi_load("data/textures/crate.png", &width, &height, &channels, 0);
	if (!texture_data)
	{
		error("Texture Loading Error", "Could not found file data/textures/crate.png.");
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
	const int uniform_model = glGetUniformLocation(program, "cModel");
	if (uniform_model < 0)
	{
		error("Shader Uniform Error", "Could not found uniform cModel in shader.");
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	const int uniform_viewproj = glGetUniformLocation(program, "cViewProj");
	if (uniform_model < 0)
	{
		error("Shader Uniform Error", "Could not found uniform cViewProj in shader.");
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "sTexture"), 0);
	glUseProgram(0);
	if (!validate_gl("Shader Uniforms Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// =====================================
	// Rendering
	// =====================================
	// View Matrix
	mat4 view;
	glm_mat4_identity(view);

	vec3 view_position = { 0.0f, 0.0f, -3.0f };
	glm_translate(view, view_position);

	// Projection Matrix
	mat4 proj;
	glm_perspective(45.0f, 1024.0f / 720.0f, 0.01f, 100.0f, proj);

	// View and Projection Matrix
	mat4 viewproj;
	glm_mat4_mul_sse2(proj, view, viewproj);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_viewproj, 1, GL_FALSE, viewproj[0]);
	glUseProgram(0);

	// Model Matrix
	mat4 model;

	// Rotation
	vec3 rotation_axis = { 0.5f, 1.0f, 0.75f };
	versor rotation;
	glm_quat_identity(rotation);

	SDL_Event event;
	int run = 1;
	int i;
	while (run)
	{
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				run = 0;

		glm_quatv(rotation, (float)SDL_GetTicks() * 0.001f, rotation_axis);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		for (i = 0; i < sizeof(positions) / sizeof(vec3); ++i)
		{
			glm_mat4_identity(model);
			glm_translate(model, positions[i]);
			glm_quat_rotate(model, rotation, model);

			glUniformMatrix4fv(uniform_model, 1, GL_FALSE, model[0]);
			glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(float), GL_UNSIGNED_INT, 0);
		}

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

static void error(const char* title, const char* format, ...)
{
	char message[ERROR_BUFFER_SIZE];
	va_list arg;
	va_start(arg, format);
	vsprintf(message, format, arg);
	va_end(arg);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
}

static int validate_gl(const char* title)
{
	static const char* GL_ERROR_MESSAGES[] =
	{
		"An unacceptable value is specified for an enumerated argument.\nThe offending command is ignored and has no other side effect than to set the error flag.",
		"A numeric argument is out of range. The offending command is ignored\nand has no other side effect than to set the error flag.",
		"The specified operation is not allowed in the current state.\nThe offending command is ignored and has no other side effect than to set the error flag.",
		"The framebuffer object is not complete. The offending command is ignored\nand has no other side effect than to set the error flag.",
		"There is not enough memory left to execute the command. The state of the GL is undefined,\nexcept for the state of the error flags, after this error is recorded.",
		"An attempt has been made to perform an operation that would cause an internal stack to underflow.",
		"An attempt has been made to perform an operation that would cause an internal stack to overflow."
	};

	int index;
	GLenum status = glGetError();
	if (status == GL_NO_ERROR)
		return 1;
	else
	{
		switch (status)
		{
		case GL_INVALID_ENUM:
			index = 0;
			break;
		case GL_INVALID_VALUE:
			index = 1;
			break;
		case GL_INVALID_OPERATION:
			index = 2;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			index = 3;
			break;
		case GL_OUT_OF_MEMORY:
			index = 4;
			break;
		case GL_STACK_UNDERFLOW:
			index = 5;
			break;
		case GL_STACK_OVERFLOW:
			index = 6;
			break;
		default:
			return 0;
		}
		error(title, GL_ERROR_MESSAGES[index]);
		return 0;
	}
}

static int load_shaders_text(char** vertex_shader, char** fragment_shader, const char* filename)
{
	char shadername[FILENAME_BUFFER_SIZE];

	sprintf(shadername, "%s.vs.glsl", filename);
	FILE* file = fopen(shadername, "r");
	if (!file)
	{
		error("Shader Loading Error", "Failed to open file %s.", shadername);
		return 0;
	}
	fseek(file, 0, SEEK_END);
	unsigned size = ftell(file);
	if (!size)
	{
		error("Shader Loading Error", "File %s is empty.", shadername);
		fclose(file);
		return 0;
	}
	rewind(file);
	*vertex_shader = (char*)malloc(size + 1);
	fread(*vertex_shader, 1, size, file);
	(*vertex_shader)[size] = '\0';
	fclose(file);

	sprintf(shadername, "%s.fs.glsl", filename);
	file = fopen(shadername, "r");
	if (!file)
	{
		error("Shader Loading Error", "Failed to open file %s.", shadername);
		free(*vertex_shader);
		*vertex_shader = NULL;
		return 0;
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (!size)
	{
		error("Shader Loading Error", "File %s is empty.", shadername);
		free(*vertex_shader);
		*vertex_shader = NULL;
		fclose(file);
		return 0;
	}
	rewind(file);
	*fragment_shader = (char*)malloc(size + 1);
	fread(*fragment_shader, 1, size, file);
	(*fragment_shader)[size] = '\0';
	fclose(file);
	return 1;
}

__declspec(dllexport) unsigned NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;