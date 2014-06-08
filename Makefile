CC = gcc
OBJ = RotatingMobile.o LoadShader.o Matrix.o Util.o ObjectGl.o OBJParserMobile.o Wall.o Billboard.o LoadTexture.o
CFLAGS = -g -Wall -Wextra -std="c99"
LDLIBS=-lm -lglut -lGLEW -lGL

TARGET = RotatingMobile

SRC_DIR = src
BUILD_DIR = build
VPATH = src

$(TARGET): $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(LDLIBS) -c $^ -o $@

clean:
	rm -f $(BUILD_DIR)/*.o *.o $(TARGET) 
.PHONY: all clean
