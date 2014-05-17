#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef __OBJECTGL_H__
#define __OBJECTGL_H__

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum DataID {vPosition = 0, vNormal = 1}; 

typedef struct {
	float model_matrix[16];

	//buffer objects
	GLuint vbo;
	GLuint cbo;
	GLuint ibo;

	GLfloat  *vertx_buffer_data;
	GLfloat  *color_buffer_data;
	GLushort *index_buffer_data;
	GLfloat  *normals;

	int num_vertx;
	int num_vectr;
	int vertx_per_vectr;
	
	//rotation parameters
	short rotation_dir;
	float rotation_dur;
	float rotation_spd;
} object_gl;


void init_object(object_gl *object);
void draw_single(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program);
void draw_n     (object_gl **objects, int n, float *proj_matrix, float *view_matrix, GLuint shader_program);
void orbit_object (object_gl *object, object_gl *center, float degree);
void rotate_object(object_gl *object, float degree);

#endif
