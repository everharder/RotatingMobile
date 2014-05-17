#include <math.h>

#ifndef __UTIL_H__
#define __UTIL_H__


float dotProduct(GLfloat *v1, GLfloat *v2);
GLfloat* crossProduct(GLfloat *v1, GLfloat *v2);
GLfloat* sumVectr(GLfloat *v1, GLfloat *v2);
GLfloat* diffVectr(GLfloat *v1, GLfloat *v2);
GLfloat* mulVectr(GLfloat *v, float c);


#endif
