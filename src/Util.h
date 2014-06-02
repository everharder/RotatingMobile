#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"

#ifndef __UTIL_H__
#define __UTIL_H__

void RGBtoHSV(GLfloat rgb[4], GLfloat hsv[4]);
void HSVtoRGB(GLfloat hsv[4], GLfloat rgb[4]);
char* getRandomTexture();


#endif
