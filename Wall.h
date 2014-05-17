#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"
#include "OBJParserMobile.h"

#ifndef __WALL_H__
#define __WALL_H__

object_gl* init_wall();
object_gl* create_wall(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB);


#endif
