#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadTexture.h"

#ifndef __OBJECTGL_H__
#define __OBJECTGL_H__

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define REFL_FACTOR_AMBIENT 	0.4
#define REFL_FACTOR_DIFFUSE	0.4
#define REFL_FACTOR_SPECULAR	1.0 

enum DataID {vPosition = 0,  vTexture = 1, vNormal = 2}; 

typedef struct 
{
    GLfloat Position[3];
    GLfloat UV[2];
} VertexData;

typedef struct {
	float model_matrix[16];

	//buffer objects
	GLuint vbo;
	GLuint nbo;
	GLuint cbo;
	GLuint ibo;

	GLfloat  *vertx_buffer_data;
	GLfloat  *color_buffer_data;
	GLushort *index_buffer_data;
	GLfloat  *normal_buffer_data;

	VertexData *vertx_texture;
	TextureDataPtr texture;
	GLuint texture_id;

	int num_vertx;
	int num_vectr;
	int vertx_per_vectr;
	
	//rotation parameters
	short rotation_dir;
	float rotation_dur;
	float rotation_spd;
} object_gl;

typedef struct lightsource {
	GLfloat intensity[4];
	//reflection
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];

	//position
	GLfloat position[4];

	int flag_ambient;
	int flag_diffuse;
	int flag_specular;
} lightsource;

void init_object(object_gl *object);
void draw_single(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights);
void draw_single_scaled(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights, double *scale);
void draw_n     (object_gl **objects, int n, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights);
void setup_texture(object_gl *object);
void orbit_object (object_gl *object, object_gl *center, float degree);
void rotate_object(object_gl *object, float degree);

#endif
