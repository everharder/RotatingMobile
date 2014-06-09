#include "ObjectGl.h"

#ifndef __BILLBOARD_H__
#define __BILLBOARD_H__

#define UV_OFFSET 0.08
#define STEP_X	0.25
#define STEP_Y	0.33
#define ANIMATE_TIME 75 //ms

object_gl* init_board();
object_gl* create_board(float x1, float y1, float z1, float x2, float y2, float z2);


#endif






