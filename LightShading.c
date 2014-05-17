#include "LightShading.h"
#include "Util.h"
#include "ObjectGl.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// All equations determine intensities for ONE TRIANGLE!!!

/******************************************************************
* diffuseReflection
*
* determines reflected light intensity of diffuse reflection
*******************************************************************/
float diffuseReflection(float intL, GLfloat *vectrL, GLfloat *vectrN, float kD){
	return kD * intL * dotProduct(vectrN, vectrL);
}

/******************************************************************
* specularReflection
*
* determines reflected light intensity of specular reflection
*******************************************************************/
float specularReflection(float intL, GLfloat *vectrL, GLfloat *vectrN, GLfloat *vectrV, float kS, float m){
	GLfloat vertxR = diffVectr(mulVectr(mulVectr(vertxN, 2), dotProduct(vertxN, vertxL)), vertxL);

	return kS * intL * pow(dotProduct(vertxR, vertxV), m);
}

/******************************************************************
* ambientReflection
*
* determines reflected light intensity of ambient reflection
*******************************************************************/
float ambientReflection(float intL, float kA){
	return intL * kA;
}

/******************************************************************
* phongLightModel
*
* determines reflected light intensity of phong lighting model
*******************************************************************/
float phongLightModel(GLfloat *vectrL, GLfloat *vectrN, GLfloat *vectrV, float intL, int numOfLights, float kD, float kS, float kA, float m){
	float intTemp = 0;

	// This step must be evaluated for all light sources!!!
	intTemp += diffuseReflection(intL, vectrL, vectrN, kD) + specularReflection(intL, vectrL, vectrN, vectrV, kS, m);
	// Determine ambientReflection only for one specific light source!!!
	return ambientReflection(intL, kA) + intTemp;
}

void gouraudShading(object_gl *object){
	// Go through all triangles of object.
	for (int i = 0; i < object->vertx_per_vectr * object->num_vectr; i+=object->vertx_per_vectr){
		// Get indices of vertices which form one triangle.
		int index1 = object->index_buffer_data[i];
		int index2 = object->index_buffer_data[i+1];
		int index3 = object->index_buffer_data[i+2];

		GLfloat *vectr1 = calloc(object->vertx_per_vectr, sizeof(GLfloat));
		GLfloat *vectr2 = calloc(object->vertx_per_vectr, sizeof(GLfloat));
		GLfloat *vectr3 = calloc(object->vertx_per_vectr, sizeof(GLfloat));

		// Determine vector for each vertex in one triangle.
		for (int j = 0; j < object->vertx_per_vectr; j++){
			vectr1[j] = object->vertx_buffer_data[index1 * object->vertx_per_vectr + j);
			vectr2[j] = object->vertx_buffer_data[index2 * object->vertx_per_vectr + j);
			vectr3[j] = object->vertx_buffer_data[index3 * object->vertx_per_vectr + j);
		}

		GLfloat *n1 = crossProduct
	}

}
