#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"

#ifndef __LIGHTSHADING_H__
#define __LIGHTSHADING_H__

// Later on: Use crossProduct(v1,v2) to determine vectrN in all reflections!!!
float diffuseReflection(float intL, GLfloat *vectrL, GLfloat *vectrN, float kD);
float specularReflection(float intL, GLfloat *vectrL, GLfloat *vectrN, GLfloat *vectrV, float kS, float m);
float ambientReflection(float intL, float kA);
float phongLightModel(GLfloat *vectrL, GLfloat *vectrN, GLfloat *vectrV, float intL, int numOfLights, float kD, float kS, float kA, float m);


#endif
