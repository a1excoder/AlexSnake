CC = gcc
FLAGS = -Wall --std=c99

CODE_FILE_NAME = main.c
END_FILE_NAME = AlexSnake

SDL = -IC:\dev_libs\SDL\x86_64-w64-mingw32\include\SDL2 -LC:\dev_libs\SDL\x86_64-w64-mingw32\lib -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2
# SDL_TTF = -IC:\dev_libs\SDL2_ttf-2.20.1\x86_64-w64-mingw32\include\SDL2 -LC:\dev_libs\SDL2_ttf-2.20.1\x86_64-w64-mingw32\lib -lSDL2_ttf
# SDL_IMG = -IC:\dev_libs\SDL2_image-2.6.2\x86_64-w64-mingw32\include\SDL2 -LC:\dev_libs\SDL2_image-2.6.2\x86_64-w64-mingw32\lib -lSDL2_image

all:
	windres set_icon.rc set_icon.o
	$(CC) $(CODE_FILE_NAME) set_icon.o $(SDL) $(FLAGS) -o $(END_FILE_NAME)
	del set_icon.o
