#include "ObjectGl.h"
#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************
* Init Object
*
* binds buffers to object
*******************************************************************/
void init_object(object_gl *object) {
	glGenBuffers(1, &(object->vbo));
	glGenBuffers(1, &(object->nbo));
	glGenBuffers(1, &(object->ibo));

	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->vertx_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, object->nbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->normal_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->num_vectr * object->vertx_per_vectr * sizeof(GLushort), object->index_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, object->cbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->color_buffer_data, GL_STATIC_DRAW);	
}

/******************************************************************
* DrawSingle
*******************************************************************/
void draw_single(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program) {
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//init normals
/*	GLfloat *normals = malloc(object->num_vertx * 3);
	
	normals = object->normals;
	for(int i=0; i < object->num_vertx; i++) {
		normals[i*3 + 0] = 0.0;
		normals[i*3 + 1] = 0.0;
		normals[i*3 + 2] = 1.0;
	}
*/
	glEnableVertexAttribArray(vNormal);
	glBindBuffer(GL_ARRAY_BUFFER, object->nbo);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT,GL_FALSE, 0, 0);

	GLint size; 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	/* Associate program with shader_program matrices */
	GLint projectionUniform = glGetUniformLocation(shader_program, "ProjectionMatrix");
	if (projectionUniform == -1) {
		fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, proj_matrix);

	GLint ViewUniform = glGetUniformLocation(shader_program, "ViewMatrix");

	if (ViewUniform == -1) {
		fprintf(stderr, "Could not bind uniform view_matrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, view_matrix);

	GLint RotationUniform = glGetUniformLocation(shader_program, "ModelMatrix");
	if (RotationUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, object->model_matrix);  

	/* Issue draw command, using indexed triangle list */
	if(object->vertx_per_vectr == 2)
		glDrawElements(GL_LINES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	else
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

/******************************************************************
* Draw N
*
* draws n objects
*******************************************************************/
void draw_n(object_gl **objects, int n, float *proj_matrix, float *view_matrix, GLuint shader_program){
	for(int i=0; i < n; i++)
		draw_single(objects[i], proj_matrix, view_matrix, shader_program);
}

/******************************************************************
* Orbit Objects
*
* let object rotate around center
*******************************************************************/
void orbit_object(object_gl *object, object_gl *center, float degree) {
	float rotation[16];
	float translat[16];
	float x = center->model_matrix[ 3];
	float y = center->model_matrix[ 7];
	float z = center->model_matrix[11];

	SetTranslation( -x, -y, -z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	

	SetRotationY(degree, rotation);
	MultiplyMatrix(rotation, object->model_matrix, object->model_matrix);

	SetTranslation( x, y, z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	
}

/******************************************************************
* rotate object
*
* This function rotates a 3d object by the value of {degree}
*******************************************************************/
void rotate_object(object_gl *object, float degree) {
	float rotation[16];
	float translat[16];
	float x = object->model_matrix[ 3];
	float y = object->model_matrix[ 7];
	float z = object->model_matrix[11];

	SetTranslation( -x, -y, -z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	
	
	SetRotationY(degree, rotation);
	MultiplyMatrix(rotation, object->model_matrix, object->model_matrix);

	SetTranslation( x, y, z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	
}
