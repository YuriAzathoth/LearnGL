#include <stdio.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#define ERROR_BUFFER_SIZE 2048

static void error(const char* title, const char* format, ...);
static int validate_gl(const char* title);

static const float vertices_a[] =
{
	-0.5f, -0.5f,  0.0f,
	 0.5f, -0.5f,  0.0f,
	 0.5f,  0.5f,  0.0f
};

static const float vertices_b[] =
{
	-0.5f, -0.5f,  0.0f,
	-0.5f,  0.5f,  0.0f,
	 0.5f,  0.5f,  0.0f
};

static const char* vertex_shader =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = vec4(aPos, 1.0);\n"
	"}";

static const char* fragment_shader1 =
	"#version 330 core\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
	"}";

static const char* fragment_shader2 =
	"#version 330 core\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
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

	// Vertex Buffers
	unsigned vao1;
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);
	if (!validate_gl("VAO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned vbo1;
	glGenBuffers(1, &vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_a), vertices_a, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	if (!validate_gl("VBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned vao2;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	if (!validate_gl("VAO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned vbo2;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_b), vertices_b, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	if (!validate_gl("VBO Creation Error"))
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

	const unsigned fragment1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment1, 1, (const char* const*)&fragment_shader1, NULL);
	glCompileShader(fragment1);
	glGetShaderiv(fragment1, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(fragment1, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	const unsigned program1 = glCreateProgram();
	glAttachShader(program1, vertex);
	glAttachShader(program1, fragment1);
	glLinkProgram(program1);
	glGetProgramiv(program1, GL_LINK_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetProgramInfoLog(program1, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	const unsigned fragment2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment2, 1, (const char* const*)&fragment_shader2, NULL);
	glCompileShader(fragment2);
	glGetShaderiv(fragment2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(fragment2, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	const unsigned program2 = glCreateProgram();
	glAttachShader(program2, vertex);
	glAttachShader(program2, fragment2);
	glLinkProgram(program2);
	glGetProgramiv(program2, GL_LINK_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetProgramInfoLog(program2, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment1);
	glDeleteShader(fragment2);

	// =====================================
	// Rendering
	// =====================================
	SDL_Event event;
	int run = 1;
	while (run)
	{
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				run = 0;

		glUseProgram(program1);
		glBindVertexArray(vao1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(program2);
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		if (validate_gl("Open GL Rendering Error"))
			SDL_GL_SwapWindow(window);
		else
			run = 0;
	}

	// =====================================
	// Destruction
	// =====================================
	// Shader
	glDeleteProgram(program1);
	glDeleteProgram(program2);

	// Vertex Buffers
	glDeleteVertexArrays(1, &vao1);
	glDeleteBuffers(1, &vbo1);
	glDeleteVertexArrays(1, &vao2);
	glDeleteBuffers(1, &vbo2);

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

__declspec(dllexport) unsigned NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
