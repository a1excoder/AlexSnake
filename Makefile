CC = gcc
FLAGS = -Wall --std=c99

CODE_FILE_NAME = main.c
END_FILE_NAME = AlexSnake
UNAME_S := $(shell uname -s)

all:
	gcc $(CODE_FILE_NAME) -lSDL2 $(FLAGS) -o $(END_FILE_NAME)

