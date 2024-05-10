OBJS = main.cpp cpu.cpp display.cpp

CC = g++

LINKED_FLAGS = -lmingw32 -lSDL2main -lSDL2

OBJ_NAME = chip8_emulator.exe

all : $(OBJS)
	$(CC) $(OBJS) $(LINKED_FLAGS) -o $(OBJ_NAME)