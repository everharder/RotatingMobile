CC = gcc
OBJ = RotatingMobile.o LoadShader.o Matrix.o Util.o ObjectGl.o OBJParserMobile.o Wall.o LoadTexture.o
CFLAGS = -g -Wall -Wextra -std="c99"

LDLIBS=-lm -lglut -lGLEW -lGL

RotatingMobile: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o RotatingMobile 
.PHONY: all clean
