#include "ObjectGl.h"
#include "Matrix.h"
#include "Util.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************
* Init Object
*
* binds buffers to object
*******************************************************************/
void init_object(object_gl *object) {
	glGenBuffers(1, &(object->vbo));
	
	glGenBuffers(1, &(object->ibo));

	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->vertx_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->num_vectr * object->vertx_per_vectr * sizeof(GLushort), object->index_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, object->cbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->color_buffer_data, GL_STATIC_DRAW);

	
}

/******************************************************************
* DrawSingle
*******************************************************************/
void draw_single(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights) {
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);;

	//init normals
	GLfloat *normals = malloc(object->num_vertx * 3);
	normals = object->normals;

	GLuint normals_buf = 0;
	glGenBuffers(1, &normals_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buf);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vNormal);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buf);
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

/*	GLint uniform_numlights = glGetUniformLocation(shader_program, "num_lights");
	if (uniform_numlights == -1) {
		fprintf(stderr, "Could not bind uniform num_lights\n");
		exit(-1);
	}
	glUniform1i(uniform_numlights, num_lights);*/

	GLint uniform_light_ambient  = glGetUniformLocation(shader_program, "La");
	GLint uniform_light_diffuse  = glGetUniformLocation(shader_program, "Ld");
	GLint uniform_light_specular = glGetUniformLocation(shader_program, "Ls");
	GLint uniform_light_position = glGetUniformLocation(shader_program, "Lp");
	if(uniform_light_ambient == -1 || uniform_light_specular == -1 || 
	   uniform_light_diffuse == -1 || uniform_light_position == -1) {
		fprintf(stderr, "Could not bind uniforms for lighting\n");
		exit(-1);
	}

	GLfloat lights_ambient [4 * num_lights];
	GLfloat lights_diffuse [4 * num_lights];
	GLfloat lights_specular[4 * num_lights];
	GLfloat lights_position[4 * num_lights];
	for(int i=0; i < num_lights; i++) {
		memcpy(&(lights_ambient  [i*4]), &(light[i].ambient), 4 * sizeof(GLfloat));
		memcpy(&(lights_diffuse  [i*4]), &(light[i].diffuse), 4 * sizeof(GLfloat));
		memcpy(&(lights_specular [i*4]), &(light[i].specular),4 * sizeof(GLfloat));
		memcpy(&(lights_position [i*4]), &(light[i].position),4 * sizeof(GLfloat));
 	}

	glUniform4fv(uniform_light_ambient,  num_lights, lights_ambient);
	glUniform4fv(uniform_light_diffuse,  num_lights, lights_diffuse); 
	glUniform4fv(uniform_light_specular, num_lights, lights_specular); 
	glUniform4fv(uniform_light_position, num_lights, lights_position);  


	GLint uniform_refl_ambient  = glGetUniformLocation(shader_program, "Ka");
	GLint uniform_refl_diffuse  = glGetUniformLocation(shader_program, "Kd");
	GLint uniform_refl_specular = glGetUniformLocation(shader_program, "Ks");
	if(uniform_refl_ambient == -1 || uniform_refl_specular == -1 || 
	   uniform_refl_diffuse == -1) {
		fprintf(stderr, "Could not bind uniforms for lighting\n");
		exit(-1);
	}

	GLfloat refl_ambient [4 * num_lights];
	GLfloat refl_diffuse [4 * num_lights];
	GLfloat refl_specular[4 * num_lights];
	for(int i=0; i < num_lights; i++) {
		refl_ambient[i*4 + 0]  = light[i].ambient[0]  * REFL_FACTOR_AMBIENT;
		refl_ambient[i*4 + 1]  = light[i].ambient[1]  * REFL_FACTOR_AMBIENT;
		refl_ambient[i*4 + 2]  = light[i].ambient[2]  * REFL_FACTOR_AMBIENT;
		refl_ambient[i*4 + 3]  = 1.0;

		refl_diffuse[i*4 + 0]  = light[i].diffuse[0]  * REFL_FACTOR_DIFFUSE;
		refl_diffuse[i*4 + 1]  = light[i].diffuse[1]  * REFL_FACTOR_DIFFUSE;
		refl_diffuse[i*4 + 2]  = light[i].diffuse[2]  * REFL_FACTOR_DIFFUSE;
		refl_diffuse[i*4 + 3]  = 1.0;

		refl_specular[i*4 + 0] = light[i].specular[0] * REFL_FACTOR_SPECULAR;
		refl_specular[i*4 + 1] = light[i].specular[1] * REFL_FACTOR_SPECULAR;
		refl_specular[i*4 + 2] = light[i].specular[2] * REFL_FACTOR_SPECULAR;
		refl_specular[i*4 + 3] = 1.0;
 	}

	glUniform4fv(uniform_refl_ambient,  num_lights, refl_ambient);
	glUniform4fv(uniform_refl_diffuse,  num_lights, refl_diffuse); 
	glUniform4fv(uniform_refl_specular, num_lights, refl_specular); 


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
void draw_n(object_gl **objects, int n, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights){
	for(int i=0; i < n; i++)
		draw_single(objects[i], proj_matrix, view_matrix, shader_program, light, num_lights);
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
