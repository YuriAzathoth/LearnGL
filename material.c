#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "cglm/affine.h"
#include "cglm/cam.h"
#include "cglm/quat.h"
#include "common.h"
#include "stb_image.h"

static const float cube_vertices[] =
{
	// Position				| Normal				| Tex Coord
	// Front
	 0.5f,  0.5f,  0.5f,	 0.0f,  0.0,  1.0,		1.0f, 1.0f,		//   0 RU
	 0.5f, -0.5f,  0.5f,	 0.0f,  0.0,  1.0,		1.0f, 0.0f,		//   1 RD
	-0.5f, -0.5f,  0.5f,	 0.0f,  0.0,  1.0,		0.0f, 0.0f,		//   2 LD
	-0.5f,  0.5f,  0.5f,	 0.0f,  0.0,  1.0,		0.0f, 1.0f,		//   3 LU

	// Back
	-0.5f,  0.5f, -0.5f,	 0.0f,  0.0, -1.0,		1.0f, 1.0f,		//   4 RU
	-0.5f, -0.5f, -0.5f,	 0.0f,  0.0, -1.0,		1.0f, 0.0f,		//   5 RD
	 0.5f, -0.5f, -0.5f,	 0.0f,  0.0, -1.0,		0.0f, 0.0f,		//   6 LD
	 0.5f,  0.5f, -0.5f,	 0.0f,  0.0, -1.0,		0.0f, 1.0f,		//   7 LU

	// Top
	 0.5f,  0.5f, -0.5f,	 0.0f,  1.0,  0.0,		1.0f, 1.0f,		//   8 RU
	 0.5f,  0.5f,  0.5f,	 0.0f,  1.0,  0.0,		1.0f, 0.0f,		//   9 RD
	-0.5f,  0.5f,  0.5f,	 0.0f,  1.0,  0.0,		0.0f, 0.0f,		//  10 LD
	-0.5f,  0.5f, -0.5f,	 0.0f,  1.0,  0.0,		0.0f, 1.0f,		//  11 LU

	// Bottom
	 0.5f, -0.5f,  0.5f,	 0.0f, -1.0,  0.0,		1.0f, 1.0f,		//  12 RU
	 0.5f, -0.5f, -0.5f,	 0.0f, -1.0,  0.0,		1.0f, 0.0f,		//  13 RD
	-0.5f, -0.5f, -0.5f,	 0.0f, -1.0,  0.0,		0.0f, 0.0f,		//  14 LD
	-0.5f, -0.5f,  0.5f,	 0.0f, -1.0,  0.0,		0.0f, 1.0f,		//  15 LU

	// Left
	-0.5f,  0.5f,  0.5f,	-1.0f,  0.0,  0.0,		1.0f, 1.0f,		//  16 LU
	-0.5f, -0.5f,  0.5f,	-1.0f,  0.0,  0.0,		1.0f, 0.0f,		//  17 LD
	-0.5f, -0.5f, -0.5f,	-1.0f,  0.0,  0.0,		0.0f, 0.0f,		//  18 RD
	-0.5f,  0.5f, -0.5f,	-1.0f,  0.0,  0.0,		0.0f, 1.0f,		//  19 RU

	// Right
	 0.5f,  0.5f, -0.5f,	 1.0f,  0.0,  0.0,		1.0f, 1.0f,		//   4 RU
	 0.5f, -0.5f, -0.5f,	 1.0f,  0.0,  0.0,		1.0f, 0.0f,		//   5 RD
	 0.5f, -0.5f,  0.5f,	 1.0f,  0.0,  0.0,		0.0f, 0.0f,		//   1 RD
	 0.5f,  0.5f,  0.5f,	 1.0f,  0.0,  0.0,		0.0f, 1.0f		//   0 RU
};

static const unsigned cube_indices[] =
{
	 0,  1,  2,  2,  3,  0,	// Front
	 4,  5,  6,  6,  7,  4,	// Back
	 8,  9, 10, 10, 11,  8,	// Top
	12, 13, 14, 14, 15, 12,	// Bottom
	16, 17, 18, 18, 19, 16,	// Left
	20, 21, 22, 22, 23, 20	// Right
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

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// STB Image
	stbi_set_flip_vertically_on_load(1);

	// Vertex Buffers

	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	if (!validate_gl("VBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	if (!validate_gl("EBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	if (!validate_gl("VBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned lamp_vao;
	glGenVertexArrays(1, &lamp_vao);
	glBindVertexArray(lamp_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	if (!validate_gl("VBO Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Shader
	int success;
	char* vertex_shader = NULL;
	char* fragment_shader = NULL;

	if (!load_shaders_text(&vertex_shader, &fragment_shader, "data/shaders/8_material"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
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

	unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
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

	const unsigned program_diffuse = glCreateProgram();
	glAttachShader(program_diffuse, vertex);
	glAttachShader(program_diffuse, fragment);
	glLinkProgram(program_diffuse);
	glGetProgramiv(program_diffuse, GL_LINK_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetProgramInfoLog(program_diffuse, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	free(vertex_shader);
	free(fragment_shader);

	if (!load_shaders_text(&vertex_shader, &fragment_shader, "data/shaders/7_emissive"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	vertex = glCreateShader(GL_VERTEX_SHADER);
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

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
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

	const unsigned program_emissive = glCreateProgram();
	glAttachShader(program_emissive, vertex);
	glAttachShader(program_emissive, fragment);
	glLinkProgram(program_emissive);
	glGetProgramiv(program_emissive, GL_LINK_STATUS, &success);
	if (!success)
	{
		char message[ERROR_BUFFER_SIZE];
		glGetProgramInfoLog(program_emissive, ERROR_BUFFER_SIZE, NULL, message);
		error("Fragment Shader Error", message);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	free(vertex_shader);
	free(fragment_shader);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Textures
	unsigned texture_diffuse;
	glGenTextures(1, &texture_diffuse);
	glBindTexture(GL_TEXTURE_2D, texture_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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
	if (!validate_gl("Texture Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	unsigned texture_specular;
	glGenTextures(1, &texture_specular);
	glBindTexture(GL_TEXTURE_2D, texture_specular);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture_data = stbi_load("data/textures/crate_specular.png", &width, &height, &channels, 0);
	if (!texture_data)
	{
		error("Texture Loading Error", "Could not found file data/textures/crate_specular.png.");
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_data);
	if (!validate_gl("Texture Creation Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Shader Uniforms
	const int uniform_model = glGetUniformLocation(program_diffuse, "cModel");
	const int uniform_model_inv = glGetUniformLocation(program_diffuse, "cModelInv");
	const int uniform_viewproj = glGetUniformLocation(program_diffuse, "cViewProj");
	const int uniform_view_pos = glGetUniformLocation(program_diffuse, "cViewPos");
	const int uniform_ambient_color = glGetUniformLocation(program_diffuse, "cAmbientColor");
	const int uniform_shininess = glGetUniformLocation(program_diffuse, "cMaterial.shininess");

	const int uniform_light_position = glGetUniformLocation(program_diffuse, "cLight.position");
	const int uniform_light_diffuse = glGetUniformLocation(program_diffuse, "cLight.diffuse");
	const int uniform_light_specular = glGetUniformLocation(program_diffuse, "cLight.specular");

	const int uniform_model_dif = glGetUniformLocation(program_emissive, "cModel");
	const int uniform_viewproj_dif = glGetUniformLocation(program_emissive, "cViewProj");
	const int uniform_color = glGetUniformLocation(program_emissive, "cColor");

	glUseProgram(program_diffuse);
	glUniform1i(glGetUniformLocation(program_diffuse, "cMaterial.sDiffuse"), 0);
	glUniform1i(glGetUniformLocation(program_diffuse, "cMaterial.sSpecular"), 1);
	glUseProgram(0);

	if (!validate_gl("Shader Uniforms Error"))
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// =====================================
	// Scene
	// =====================================
	// Cube
	vec3 cube_position = { 0.0f, 0.0f, 0.0f };
	vec3 cube_axis = { 0.5, 0.5, 0.2 };
	const float cube_shininess = 32.0f;
	versor cube_rotation = GLM_QUAT_IDENTITY_INIT;

	// Light
	vec3 ambient_color = { 0.2f, 0.2f, 0.2f };
	vec3 light_diffuse = { 1.0f, 0.8f, 0.6f };
	vec3 light_specular = { 0.5f, 0.5f, 0.5f };
	vec3 light_distance = { 2.0f, 1.0f, -0.5f };
	vec3 light_scale = { 0.25f, 0.25f, 0.25f };
	vec3 light_axis = { 0.0f, 0.8f, 0.2f };
	vec3 light_position;
	versor light_rotation = GLM_QUAT_IDENTITY_INIT;

	// Camera
	vec3 camera_position = { 0.0f, 0.0f, 3.0f };
	vec3 camera_direction;
	vec3 camera_up;
	vec3 camera_move;
	versor camera_rotation = GLM_QUAT_IDENTITY_INIT;
	versor camera_rotate;

	// =====================================
	// Rendering
	// =====================================
	// Matrices
	mat4 model, model_inv, view, viewproj;

	// Projection Matrix
	mat4 proj;
	glm_perspective(45.0f, 1024.0f / 720.0f, 0.01f, 100.0f, proj);

	// Rotation
	vec3 rotation_axis = { 0.5f, 1.0f, 0.75f };
	versor rotation;
	glm_quat_identity(rotation);

	SDL_Event event;
	int run = 1;
	float tick_delta;
	float tick_curr;
	float tick_prev = 0.0f;
	unsigned short controls = 0;
	while (run)
	{
		tick_curr = (float)SDL_GetTicks();
		tick_delta = tick_curr - tick_prev;
		process_events(camera_position, camera_direction, camera_rotation, &controls, &run, tick_delta);
		tick_prev = tick_curr;

		// =================================
		// Camera
		// =================================
		// Look
		glm_quat_rotatev(camera_rotation, GLM_FORWARD, camera_direction);

		// View Matrix
		glm_quat_rotatev(camera_rotation, GLM_YUP, camera_up);
		glm_look(camera_position, camera_direction, camera_up, view);

		// View and Projection Matrix
		glm_mat4_mul_sse2(proj, view, viewproj);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_quatv(rotation, tick_delta * 0.001f, light_axis);
		glm_quat_mul_sse2(rotation, light_rotation, light_rotation);
		glm_quat_rotatev(light_rotation, light_distance, light_position);

		glm_mat4_identity(model);
		glm_translate(model, cube_position);
		glm_quatv(rotation, tick_delta * -0.00025f, cube_axis);
		glm_quat_mul_sse2(rotation, cube_rotation, cube_rotation);
		glm_quat_rotate(model, cube_rotation, model);

		glm_mat4_inv_sse2(model, model_inv);
		glm_mat4_transp_sse2(model_inv, model_inv);

		glUseProgram(program_diffuse);
		glUniformMatrix4fv(uniform_viewproj, 1, GL_FALSE, viewproj[0]);
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, model[0]);
		glUniformMatrix4fv(uniform_model_inv, 1, GL_FALSE, model_inv[0]);
		glUniformMatrix4fv(uniform_model_inv, 1, GL_FALSE, model_inv[0]);
		glUniform1f(uniform_shininess, cube_shininess);
		glUniform3fv(uniform_light_position, 1, light_position);
		glUniform3fv(uniform_light_diffuse, 1, light_diffuse);
		glUniform3fv(uniform_light_specular, 1, light_specular);
		glUniform3fv(uniform_ambient_color, 1, ambient_color);
		glUniform3fv(uniform_view_pos, 1, camera_position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_specular);
		glBindVertexArray(cube_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, sizeof(cube_vertices) / sizeof(float), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm_mat4_identity(model);
		glm_quat_rotate(model, rotation, model);
		glm_translate(model, light_position);
		glm_scale(model, light_scale);

		glUseProgram(program_emissive);
		glUniformMatrix4fv(uniform_viewproj_dif, 1, GL_FALSE, viewproj[0]);
		glUniformMatrix4fv(uniform_model_dif, 1, GL_FALSE, model[0]);
		glUniform3fv(uniform_color, 1, light_diffuse);
		glBindVertexArray(lamp_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, sizeof(cube_vertices) / sizeof(float), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (validate_gl("Open GL Rendering Error"))
			SDL_GL_SwapWindow(window);
		else
			run = 0;
	}

	// =====================================
	// Destruction
	// =====================================
	// Texture
	glDeleteTextures(1, &texture_diffuse);
	glDeleteTextures(1, &texture_specular);

	// Shader
	glDeleteProgram(program_emissive);
	glDeleteProgram(program_diffuse);

	// Vertex Buffers
	glDeleteVertexArrays(1, &lamp_vao);
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteBuffers(1, &vbo);

	// SDL
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

__declspec(dllexport) unsigned NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
