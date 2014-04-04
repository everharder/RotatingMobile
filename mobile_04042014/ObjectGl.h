#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef __OBJECTGL_H__
#define __OBJECTGL_H__

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum DataID {vPosition = 0, vColor = 1}; 

typedef struct {
	float model_matrix[16];

	//buffer objects
	GLuint vbo;
	GLuint cbo;
	GLuint ibo;

	GLfloat  *vertx_buffer_data;
	GLfloat  *color_buffer_data;
	GLushort *index_buffer_data;

	int num_vertx;
	int num_vectr;
	int vertx_per_vectr;
	
	//rotation parameters
	short rotation_dir;
	float rotation_dur;
	float rotation_spd;

	//position data for translation
	float x;
	float y;
	float z;
} object_gl;



void draw_single(object_gl object, float *ProjMatrix, float *ViewMatrix, GLuint shader);
void orbit_object (object_gl *object, float degree, float x, float y, float z);
void rotate_object(object_gl *object, float degree);
void rotate_object_native(object_gl *object);

#endif
