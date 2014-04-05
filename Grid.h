#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"
#include "OBJParserMobile.h"

#ifndef __GRID_H__
#define __GRID_H__

void init_grid(object_gl *grid, int vertices);
object_gl* create_gridXY(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);
object_gl* create_gridXZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);
object_gl* create_gridYZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices);
node_object* parse_grid3D(object_gl objX, object_gl objY, object_gl objZ);


#endif
