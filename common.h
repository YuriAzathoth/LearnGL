#ifndef COMMON_H
#define COMMON_H

#include "cglm/types.h"

#define ERROR_BUFFER_SIZE 2048

typedef SDL_Event SDL_Event;

void error(const char* title, const char* format, ...);
int validate_gl(const char* title);
int load_shaders_text(char** vertex_shader, char** fragment_shader, const char* filename);
void process_events(vec3 position, vec3 direction, versor rotation, unsigned short* controls, int* run, float frame_time);

#endif // COMMON_H
