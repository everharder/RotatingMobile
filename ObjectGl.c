#include "ObjectGl.h"
#include "Matrix.h"	
#include <stdio.h>
#include <stdlib.h>

/******************************************************************
* DrawSingle
*
* draws single object 
*******************************************************************/
void draw_single(object_gl object, float *ProjMatrix, float *ViewMatrix, GLuint shader) {
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);;

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, object.cbo);
	glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

	GLint size; 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ibo);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	/* Associate program with shader matrices */
	GLint projectionUniform = glGetUniformLocation(shader, "ProjectionMatrix");
	if (projectionUniform == -1) {
		fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjMatrix);

	GLint ViewUniform = glGetUniformLocation(shader, "ViewMatrix");

	if (ViewUniform == -1) {
		fprintf(stderr, "Could not bind uniform ViewMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

	GLint RotationUniform = glGetUniformLocation(shader, "ModelMatrix");
	if (RotationUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, object.model_matrix);  

	/* Issue draw command, using indexed triangle list */
	if(object.vertx_per_vectr == 2)
		glDrawElements(GL_LINES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	else
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}

void orbit_object(object_gl *object, float degree) {
	float rotation[16];

	SetRotationY(degree, rotation);
	MultiplyMatrix(rotation, object->model_matrix, object->model_matrix);
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

	SetTranslation(-x,-y,-z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	
	
	SetRotationY(degree, rotation);
	MultiplyMatrix(rotation, object->model_matrix, object->model_matrix);

	SetTranslation( x, y, z,translat);
	MultiplyMatrix(translat, object->model_matrix, object->model_matrix);	
}
