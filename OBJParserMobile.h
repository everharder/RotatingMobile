#include "ObjectGl.h"
#include "Grid.h"

#ifndef __OBJPARSERMOBILE_H__
#define __OBJPARSERMOBILE_H__

/******************************************************************
* structs
*******************************************************************/
typedef struct node_object {
	object_gl obj;
	struct node_object *child_r;
	struct node_object *child_l;
} node_object;


node_object* parse_mobile(char *filename);
node_object* parse_grid3D(object_gl objX, object_gl objY, object_gl objZ);


#endif
