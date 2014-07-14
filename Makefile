CC = gcc
OBJ = RotatingMobile.o LoadShader.o Matrix.o Util.o ObjectGl.o OBJParserMobile.o Wall.o Billboard.o LoadTexture.o
CFLAGS = -g -Wall -Wextra -std="c99"
LDLIBS=-lm -lglut -lGLEW -lGL

TARGET = RotatingMobile

SRC_DIR = src
BUILD_DIR = build
VPATH = src

# Rules
all: $(TARGET)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(LDLIBS) -c $^ -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(LDLIBS) -c $^ -o $@

clean:
	rm -f $(BUILD_DIR)/*.o *.o *.c~ *.o~ $(TARGET) 
.PHONY: all clean

# Dependencies
$(TARGET): $(BUILD_DIR)/LoadShader.o $(BUILD_DIR)/Matrix.o $(BUILD_DIR)/Util.o $(BUILD_DIR)/ObjectGl.o $(BUILD_DIR)/OBJParserMobile.o $(BUILD_DIR)/Wall.o $(BUILD_DIR)/Billboard.o $(BUILD_DIR)/LoadTexture.o | $(BUILD_DIR)
