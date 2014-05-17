#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"

#ifndef __UTIL_H__
#define __UTIL_H__


float dotProduct(GLfloat *v1, GLfloat *v2);
void crossProduct(GLfloat *v1, GLfloat *v2, GLfloat* result);
void sumVectr(GLfloat *v1, GLfloat *v2, GLfloat* result);
void diffVectr(GLfloat *v1, GLfloat *v2, GLfloat* result);
void mulVectr(GLfloat *v, float c, GLfloat* result);
void findNormals(object_gl *object);


#endif
