#include <stdlib.h>
#include <time.h>
#include "Util.h"

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]

/******************************************************************
* RGBtoHSV
*
* converts from RGB to HSV color space.
*******************************************************************/
void RGBtoHSV(GLfloat rgb[4], GLfloat hsv[4]){
	GLfloat r, g, b, min, max, delta;
	
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];

	min = fmin(fmin(r, g), b);
	max = fmax(fmax(r, g), b);
	delta = max - min;

	if (max == min)
		hsv[0] = 0;
	else if (max == r)
		hsv[0] = 60.0 * (g - b) / delta;	// between yellow & magenta
	else if(max == g)
		hsv[0] = 60.0 * (2.0 + (b - r) / delta);	// between cyan & yellow
	else
		hsv[0] = 60.0 * (4.0 + (r - g) / delta);	// between magenta & cyan

	if(hsv[0] < 0)	hsv[0] += 360.0;

/*
	if(max == 0)
		// r = g = b = 0		
		hsv[1] = 0;			// s = 0
	else
		hsv[1] = delta / max;		// s
*/
	hsv[1] = 1;		// fully saturation		
	hsv[2] = max;		// v
	hsv[3] = rgb[3];
}

/******************************************************************
* HSVtoRGB
*
* converts from HSV to RGB color space.
*******************************************************************/
void HSVtoRGB(GLfloat hsv[4], GLfloat rgb[4]){
	GLfloat h, s, v, f, p, q, t;
	int i;

	h = hsv[0];
	s = 1;		// fully saturation
	v = hsv[2];

	if(s == 0) {
		// achromatic (grey)
		rgb[0] = v;
		rgb[1] = v;
		rgb[2] = v;
	}
	else {
		h = h / 60;			// sector 0 to 5
		i = floor(h);
		f = h - i;			// factorial part of h
		p = v * (1 - s);
		q = v * (1 - s * f);
		t = v * (1 - s * (1 - f));

		switch(i) {
			case 0:
				rgb[0] = v;
				rgb[1] = t;
				rgb[2] = p;
				break;
			case 1:
				rgb[0] = q;
				rgb[1] = v;
				rgb[2] = p;
				break;
			case 2:
				rgb[0] = p;
				rgb[1] = v;
				rgb[2] = t;
				break;
			case 3:
				rgb[0] = p;
				rgb[1] = q;
				rgb[2] = v;
				break;
			case 4:
				rgb[0] = t;
				rgb[1] = p;
				rgb[2] = v;
				break;
			case 5:
				rgb[0] = v;
				rgb[1] = p;
				rgb[2] = q;
				break;
			default:
				break;
		}
	}

	rgb[3] = hsv[3];
}

/******************************************************************
*
* setupTexture
*
* This function is called to load the texture and initialize
* texturing parameters for each object.
*
*******************************************************************/
void setup_texture(object_gl *object, char *filename)
{	
    /* Allocate texture container */
    object->texture = malloc(sizeof(TextureDataPtr));

    int success = LoadTexture(filename, object->texture);
    if (!success)
    {
        printf("Error loading texture '%s'. Exiting.\n", filename);
	exit(-1);
    }

    /* Create texture name and store in handle */
    glGenTextures(1, &(object->texture_id));
	
    /* Bind texture */
    glBindTexture(GL_TEXTURE_2D, object->texture_id);

    /* Load texture image into memory */
    glTexImage2D(GL_TEXTURE_2D,     /* Target texture */
		 0,                 /* Base level */
		 GL_RGB,            /* Each element is RGB triple */ 
		 object->texture->width, object->texture->height, 
		 0,                 /* Border should be zero */
		 GL_BGR,            /* Data storage format */
		 GL_UNSIGNED_BYTE,  /* Type of pixel data */
		 object->texture->data);    /* Pointer to image data  */

    /* Repeat texture on edges when tiling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* Linear interpolation for magnification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Trilinear MIP mapping for minification */ 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glGenerateMipmap(GL_TEXTURE_2D); 
}

/******************************************************************
* getRandomTexture
*
* returns a random picture for texturing out of an existing list.
*******************************************************************/
char* getRandomTexture(){
	srand(time(NULL));

	switch(rand() % 3){
		case 0: return "data/marble.bmp";
		case 1: return "data/honeycomp.bmp";
		case 2: return "data/bricks.bmp";
		default: return NULL;
	}
}
