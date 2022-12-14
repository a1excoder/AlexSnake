#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
// #include <SDL_ttf.h>
// #include <SDL_image.h>

#define BLOCK_SIZE 16
#define FIELD_SIZE 32
#define SNAKE_MAX_LENGTH 32

static const int SCREEN_SIZE[] = {BLOCK_SIZE * FIELD_SIZE, BLOCK_SIZE * FIELD_SIZE};
static enum {TO_TOP = 1, TO_DOWN = 2, TO_LEFT = 3, TO_RIGHT = 4}; // , PAUSE = 5

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
// static TTF_Font *font = NULL;
// static SDL_Color text_color = {0xFF, 0xFF, 0xFF, 0x00};
// static SDL_Surface *icon = NULL;

int init_resources();
void destroy_resources();

int main(int argc, char *argv[])
{
    int init_status = init_resources();
    if (init_status) return init_status;

//#if defined(_WIN32)
//    SDL_SetWindowTitle(window, "10/32");
//#endif

    SDL_Event event_game;
    SDL_bool quit = SDL_TRUE;
    SDL_bool is_max_length = SDL_FALSE;
    SDL_Rect rect_block = {.h = BLOCK_SIZE, .w = BLOCK_SIZE};

    int move_to = 4;
    int i_coords, j_coords;
    int game_speed_ms = 35;
    int body_snake_length = 4;
    int rewrite_snake_coords = 1;

    SDL_Point eat_spawn_coords;
    SDL_Point snake_body[SNAKE_MAX_LENGTH];
    int map_structure[FIELD_SIZE][FIELD_SIZE];

    srand(time(NULL));

    // set head coords
    snake_body[0].x = rand() % 0x20;
    snake_body[0].y = rand() % 0x20;

    // first random eat spawn
    eat_spawn_coords.x = rand() % 0x20;
    eat_spawn_coords.y = rand() % 0x20;

    for (i_coords = 0; i_coords < FIELD_SIZE; i_coords++) {
        for (j_coords = 0; j_coords < FIELD_SIZE; j_coords++) {
            map_structure[i_coords][j_coords] = 0;
        }
    }

    while (quit) {
    	while (SDL_PollEvent(&event_game) != NULL) {
            if (event_game.type == SDL_QUIT) {
                quit = SDL_FALSE;
            }

            if (event_game.type == SDL_KEYDOWN) {
                switch (event_game.key.keysym.sym) {
                    case SDLK_UP:
                        if (move_to != TO_DOWN) move_to = TO_TOP;
                        break;
                    case SDLK_DOWN:
                        if (move_to != TO_TOP) move_to = TO_DOWN;
                        break;
                    case SDLK_LEFT:
                        if (move_to != TO_RIGHT) move_to = TO_LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (move_to != TO_LEFT) move_to = TO_RIGHT;
                        break;
//                    case SDLK_ESCAPE:
//                        move_to = PAUSE;
//                        break;
                    default:
                        break;
                }

            }

        }

        switch (move_to) {
            case TO_TOP:
                snake_body[0].y--;
                break;
            case TO_DOWN:
                snake_body[0].y++;
                break;
            case TO_LEFT:
                snake_body[0].x--;
                break;
            case TO_RIGHT:
                snake_body[0].x++;
                break;
//            case PAUSE:
//                break;
            default:
                break;
        }

        if (snake_body[0].x > FIELD_SIZE-1) {
            snake_body[0].x = 0;
        }

        if (snake_body[0].y > FIELD_SIZE-1) {
            snake_body[0].y = 0;
        }

        if (snake_body[0].x < 0) {
            snake_body[0].x = FIELD_SIZE-1;
        }

        if (snake_body[0].y < 0) {
            snake_body[0].y = FIELD_SIZE-1;
        }

        // eat yourself | you lose
        for (i_coords = 1; i_coords < body_snake_length; i_coords++) {
            if (snake_body[0].x == snake_body[i_coords].x && snake_body[0].y == snake_body[i_coords].y) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "You lose", "You lose!", window);
                quit = SDL_FALSE;
            }
        }

        // if head on eat
        if (snake_body[0].x == eat_spawn_coords.x && snake_body[0].y == eat_spawn_coords.y) {
            map_structure[eat_spawn_coords.x][eat_spawn_coords.y] = 0;

            eat_spawn_coords.x = rand() % 0x20;
            eat_spawn_coords.y = rand() % 0x20;
            body_snake_length++;

            if (body_snake_length % 5 == 0) game_speed_ms -= 4;
        }

        // snake length == max(32) then game stop
        if (body_snake_length == SNAKE_MAX_LENGTH) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "You win", "You win!", window);
            quit = SDL_FALSE;
        }

        snake_body[rewrite_snake_coords].x = snake_body[0].x;
        snake_body[rewrite_snake_coords].y = snake_body[0].y;


        if (rewrite_snake_coords <= body_snake_length) rewrite_snake_coords++;
        else {
            rewrite_snake_coords = 1;
            is_max_length = SDL_TRUE;
        }

        if (is_max_length == SDL_TRUE) {
            // sometimes in snake_body[rewrite_snake_coords] x and y values > max field size or < min size
            // this crutch need for fix this bug
            // I'm actually just lazy and don't know how to use a debugger ;)
            if (snake_body[rewrite_snake_coords].x < FIELD_SIZE && snake_body[rewrite_snake_coords].x >= 0 &&
                    snake_body[rewrite_snake_coords].y < FIELD_SIZE && snake_body[rewrite_snake_coords].y >= 0) {
                map_structure[snake_body[rewrite_snake_coords].x][snake_body[rewrite_snake_coords].y] = 0;
            }
        }

        // render head
        map_structure[snake_body[0].x][snake_body[0].y] = 3;

        map_structure[eat_spawn_coords.x][eat_spawn_coords.y] = 2;

        for (i_coords = 0; i_coords < FIELD_SIZE; i_coords++) {
            for (j_coords = 0; j_coords < FIELD_SIZE; j_coords++) {
                switch (map_structure[i_coords][j_coords]) {
                    case 0:
                        // background
                        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
                        break;
                    case 1:
                        // snake body
                        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
                        break;
                    case 2:
                        // eat
                        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
                        break;
                    case 3:
                        // snake head
                        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
                        break;
                    default:
                        break;
                }
                rect_block.x = BLOCK_SIZE * i_coords;
                rect_block.y = BLOCK_SIZE * j_coords;
                SDL_RenderFillRect(renderer, &rect_block);
            }
        }

        // after rendering the head, you need to make it part of the body
        map_structure[snake_body[0].x][snake_body[0].y] = 1;

        // grid for debugging
#if 1
        static SDL_Rect debug_rect;
        for (i_coords = 0; i_coords < FIELD_SIZE; i_coords++) {
            for (j_coords = 0; j_coords < FIELD_SIZE; j_coords++) {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
                debug_rect.x = i_coords * BLOCK_SIZE;
                debug_rect.y = j_coords * BLOCK_SIZE;
                debug_rect.h = 100;
                debug_rect.w = 1;
                SDL_RenderFillRect(renderer, &debug_rect);

                debug_rect.h = 1;
                debug_rect.w = 100;
                SDL_RenderFillRect(renderer, &debug_rect);
            }
        }
#endif

    	SDL_RenderPresent(renderer);
        SDL_Delay(game_speed_ms);
    }

    destroy_resources();
	return 0;
}

int init_resources() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL init error - %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("AlexSnake - simple snake game written in C with SDL",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_SIZE[0], SCREEN_SIZE[1],
                              SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("create window error - %s\n", SDL_GetError());
        return 2;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("create renderer error - %s\n", SDL_GetError());
        return 3;
    }

//    if (TTF_Init() != 0) {
//        printf("TTF init error - %s\n", TTF_GetError());
//        return 4;
//    }
//
//    if (!(font = TTF_OpenFont("arial.ttf", 16))) {
//        printf("font open error - %s\n", TTF_GetError());
//        return 5;
//    }

//    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//        printf("IMG init error - %s\n", SDL_GetError());
//        return 5;
//    }

//    icon = IMG_Load("icon.png");
//    SDL_SetWindowIcon(window, icon);

    return 0;
}

void destroy_resources() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
//     TTF_CloseFont(font);
//     SDL_FreeSurface(icon);

//    TTF_Quit();
//    IMG_Quit();
    SDL_Quit();
}
