#ifndef COMMON_H
#define COMMON_H

#define CAMERA_SENSITIVITY -0.00125f
#define CAMERA_SENSITIVITY_MOUSE -0.00025f
#define CAMERA_SPEED 0.005f
#define CONTROL_FORWARD 0x0001
#define CONTROL_BACK 0x0002
#define CONTROL_LEFT 0x0004
#define CONTROL_RIGHT 0x0008
#define CONTROL_UP 0x0010
#define CONTROL_DOWN 0x0020
#define CONTROL_PITCH_UP 0x0040
#define CONTROL_PITCH_DOWN 0x0080
#define CONTROL_YAW_LEFT 0x0100
#define CONTROL_YAW_RIGHT 0x0200
#define CONTROL_ROLL_LEFT 0x0400
#define CONTROL_ROLL_RIGHT 0x0800
#define ERROR_BUFFER_SIZE 2048

void error(const char* title, const char* format, ...);
int validate_gl(const char* title);
int load_shaders_text(char** vertex_shader, char** fragment_shader, const char* filename);
void process_controls(unsigned type, unsigned key, unsigned short* controls);

#endif // COMMON_H
