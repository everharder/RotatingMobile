#include "ObjectGl.h"
#include "Matrix.h"
#include "LoadTexture.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************
* Init Object
*
* binds buffers to object
*******************************************************************/
void init_object(object_gl *object) {
	glGenBuffers(1, &(object->vbo));
	glGenBuffers(1, &(object->cbo));
	glGenBuffers(1, &(object->nbo));
	glGenBuffers(1, &(object->ibo));

	object->vertx_texture= malloc(sizeof(VertexData) * object->num_vertx);
	for(int i=0; i < object->num_vertx; i++) {
		object->vertx_texture[i].Position[0] = object->vertx_buffer_data[3*i + 0];
		object->vertx_texture[i].Position[1] = object->vertx_buffer_data[3*i + 1];
		object->vertx_texture[i].Position[2] = object->vertx_buffer_data[3*i + 2];

		object->vertx_texture[i].UV[0] = object->color_buffer_data[2*i + 0];
		object->vertx_texture[i].UV[1] = object->color_buffer_data[2*i + 1];
	}

  
    	/* Create new buffer object and assign name */
    	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
 
    	/* Load data into buffer object */
    	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * object->num_vertx, object->vertx_texture, GL_STATIC_DRAW);

/*	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->vertx_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, object->cbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 2 * sizeof(GLfloat), object->color_buffer_data, GL_STATIC_DRAW);*/


	glBindBuffer(GL_ARRAY_BUFFER, object->nbo);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), object->normal_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->num_vectr * object->vertx_per_vectr * sizeof(GLushort), object->index_buffer_data, GL_STATIC_DRAW);	
}

/******************************************************************
* DrawSingle
*******************************************************************/
void draw_single(object_gl *object, float *proj_matrix, float *view_matrix, GLuint shader_program, lightsource *light, int num_lights) {
	 glBindBuffer(GL_ARRAY_BUFFER, object->vbo);

	/* Activate first (and only) texture unit */
	glActiveTexture(GL_TEXTURE0);
    	/* Bind current texture  */
	glBindTexture(GL_TEXTURE_2D, object->texture_id);
	    
    	/* Get texture uniform handle from fragment shader */ 
	GLuint TextureUniform = glGetUniformLocation(shader_program, "TextureSampler");
	glUniform1i(TextureUniform, 0);
	
	/* Set location of uniform sampler variable */ 
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vTexture);	

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    	glVertexAttribPointer(vTexture, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const GLvoid*) (3 * sizeof(GLfloat))); 

	//init normals
	GLfloat *normals = malloc(object->num_vertx * 3);
	normals = object->normal_buffer_data;

	GLuint normals_buf = 0;
	glGenBuffers(1, &normals_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buf);
	glBufferData(GL_ARRAY_BUFFER, object->num_vertx * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	//put normals data
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

	GLint uniform_flag_ambient  = glGetUniformLocation(shader_program, "flag_ambient");
	GLint uniform_flag_diffuse  = glGetUniformLocation(shader_program, "flag_diffuse");
	GLint uniform_flag_specular = glGetUniformLocation(shader_program, "flag_specular");

	glUniform1i(uniform_flag_ambient,  light[0].flag_ambient);
	glUniform1i(uniform_flag_diffuse,  light[0].flag_diffuse);
	glUniform1i(uniform_flag_specular, light[0].flag_specular);


	GLint uniform_light_intensity = glGetUniformLocation(shader_program, "Li");
	GLint uniform_light_ambient   = glGetUniformLocation(shader_program, "La");
	GLint uniform_light_position  = glGetUniformLocation(shader_program, "Lp");
	GLint uniform_light_diffuse  = glGetUniformLocation(shader_program, "Ld");
	if(uniform_light_intensity == -1 || uniform_light_ambient == -1 || uniform_light_position == -1 || uniform_light_diffuse == -1) {
		fprintf(stderr, "Could not bind uniforms for lighting\n");
		exit(-1);
	}

	GLfloat lights_intensity[4 * num_lights];
	GLfloat lights_ambient  [4 * num_lights];
	GLfloat lights_position [4 * num_lights];
	GLfloat lights_diffuse  [4 * num_lights];
	for(int i=0; i < num_lights; i++) {
		memcpy(&(lights_intensity[i*4]), &(light[i].intensity), 4 * sizeof(GLfloat));
		memcpy(&(lights_ambient  [i*4]), &(light[i].ambient),   4 * sizeof(GLfloat));
		memcpy(&(lights_position [i*4]), &(light[i].position),  4 * sizeof(GLfloat));
		memcpy(&(lights_diffuse  [i*4]), &(light[i].diffuse),   4 * sizeof(GLfloat));
 	}  

	glUniform4fv(uniform_light_intensity,num_lights, lights_intensity);
	glUniform4fv(uniform_light_ambient,  num_lights, lights_ambient);
	glUniform4fv(uniform_light_position, num_lights, lights_position);
	glUniform4fv(uniform_light_diffuse,  num_lights, lights_diffuse);    


	GLint uniform_refl_ambient  = glGetUniformLocation(shader_program, "Ka");
	GLint uniform_refl_diffuse  = glGetUniformLocation(shader_program, "Kd");
	GLint uniform_refl_specular = glGetUniformLocation(shader_program, "Ks");
	if(uniform_refl_ambient == -1 || uniform_refl_specular == -1 || 
	   uniform_refl_diffuse == -1) {
		fprintf(stderr, "Could not bind uniforms for reflections\n");
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
	//glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(vTexture);   
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
