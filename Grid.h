#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef __GRID_H__
#define __GRID_H__

void init_grid(int vertices);
object_gl* create_gridXY(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);
object_gl* create_gridXZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);
object_gl* create_gridYZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);

#endif
