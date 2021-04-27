#include <stdio.h>
#include <GL/glew.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_messagebox.h>
#include "common.h"

#define FILENAME_BUFFER_SIZE 256

void error(const char* title, const char* format, ...)
{
	char message[ERROR_BUFFER_SIZE];
	va_list arg;
	va_start(arg, format);
	vsprintf(message, format, arg);
	va_end(arg);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
}

int validate_gl(const char* title)
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

int load_shaders_text(char** vertex_shader, char** fragment_shader, const char* filename)
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

void process_controls(unsigned type, unsigned key, unsigned short* controls)
{
	switch (type)
	{
	case SDL_KEYDOWN:
		switch (key)
		{
		case SDLK_w:
			*controls |= CONTROL_FORWARD;
			break;
		case SDLK_s:
			*controls |= CONTROL_BACK;
			break;
		case SDLK_a:
			*controls |= CONTROL_LEFT;
			break;
		case SDLK_d:
			*controls |= CONTROL_RIGHT;
			break;
		case SDLK_SPACE:
			*controls |= CONTROL_UP;
			break;
		case SDLK_LCTRL:
			*controls |= CONTROL_DOWN;
			break;
		case SDLK_UP:
			*controls |= CONTROL_PITCH_UP;
			break;
		case SDLK_DOWN:
			*controls |= CONTROL_PITCH_DOWN;
			break;
		case SDLK_LEFT:
			*controls |= CONTROL_YAW_LEFT;
			break;
		case SDLK_RIGHT:
			*controls |= CONTROL_YAW_RIGHT;
			break;
		case SDLK_q:
			*controls |= CONTROL_ROLL_LEFT;
			break;
		case SDLK_e:
			*controls |= CONTROL_ROLL_RIGHT;
			break;
		}
		break;
	case SDL_KEYUP:
		switch (key)
		{
		case SDLK_w:
			*controls &= ~CONTROL_FORWARD;
			break;
		case SDLK_s:
			*controls &= ~CONTROL_BACK;
			break;
		case SDLK_a:
			*controls &= ~CONTROL_LEFT;
			break;
		case SDLK_d:
			*controls &= ~CONTROL_RIGHT;
			break;
		case SDLK_SPACE:
			*controls &= ~CONTROL_UP;
			break;
		case SDLK_LCTRL:
			*controls &= ~CONTROL_DOWN;
			break;
		case SDLK_UP:
			*controls &= ~CONTROL_PITCH_UP;
			break;
		case SDLK_DOWN:
			*controls &= ~CONTROL_PITCH_DOWN;
			break;
		case SDLK_LEFT:
			*controls &= ~CONTROL_YAW_LEFT;
			break;
		case SDLK_RIGHT:
			*controls &= ~CONTROL_YAW_RIGHT;
			break;
		case SDLK_q:
			*controls &= ~CONTROL_ROLL_LEFT;
			break;
		case SDLK_e:
			*controls &= ~CONTROL_ROLL_RIGHT;
			break;
		}
	}
}
