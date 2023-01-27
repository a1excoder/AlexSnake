#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define BLOCK_SIZE 0x10
#define FIELD_SIZE 0x20
#define SNAKE_MAX_LENGTH FIELD_SIZE // 32
#define STR_BUFF_SIZE 64

// if you neen grid on map
#define GRID_ON

static const int SCREEN_SIZE[] = { BLOCK_SIZE * FIELD_SIZE, BLOCK_SIZE * FIELD_SIZE };
static enum { TO_TOP = 1, TO_DOWN = 2, TO_LEFT = 3, TO_RIGHT = 4 } move_to = TO_RIGHT;
static enum { PLAYING = 1, WIN = 2, LOSE = 3 }  game_status = PLAYING;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static TTF_Font* font = NULL;
static SDL_Color text_color = {0xFF, 0xFF, 0xFF, 0x00};

static SDL_Surface* text_surf = NULL;
static SDL_Texture* text_texture = NULL;

int init_resources(void);
void destroy_resources(void);


int main(int argc, char* argv[]) {
    SDL_Event event_game;
    SDL_bool quit = SDL_TRUE;
    SDL_bool is_max_length = SDL_FALSE;
    SDL_Rect rect_block = { .h = BLOCK_SIZE, .w = BLOCK_SIZE };
    SDL_Rect text_pos;

    int i_coords, j_coords;
    int body_snake_length = 4;
    int rewrite_snake_coords = 1;
    Uint32 game_speed = 20;
    int err_code;

    Uint32 timer_start, timer_end, timer_one_tick = 0, fps = 0;
    Uint32 game_speed_timer = SDL_GetTicks();

    const Uint32 delay_time = 1000 / 60;

    SDL_Point eat_spawn_coords;
    SDL_Point snake_body[SNAKE_MAX_LENGTH];
    int map_structure[FIELD_SIZE][FIELD_SIZE] = {{0}, {0}};
    char str_buffer[STR_BUFF_SIZE];

    SDL_zeroa(snake_body);
    // SDL_zeroa(map_structure);

#ifdef GRID_ON
    SDL_Rect grid_rect;
#endif // GRID_ON

    err_code = init_resources();
    if (err_code) return err_code;

    srand(time(NULL));

    // set head coords
    snake_body[0].x = rand() % 0x20;
    snake_body[0].y = rand() % 0x20;

    // first random eat spawn
    eat_spawn_coords.x = rand() % 0x20;
    eat_spawn_coords.y = rand() % 0x20;

    timer_start = SDL_GetTicks();

    while (quit) {
        while (SDL_PollEvent(&event_game)) {
            if (event_game.type == SDL_QUIT) {
                quit = SDL_FALSE;
            }

            if (event_game.type == SDL_KEYDOWN && game_status == PLAYING) {
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
                default:
                    break;
                }

            }

        }

        if (game_status == PLAYING && SDL_GetTicks() - game_speed_timer > game_speed) {
            game_speed_timer = SDL_GetTicks();

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
            default:
                break;
            }

            if (snake_body[0].x > FIELD_SIZE - 1) {
                snake_body[0].x = 0;
            }

            if (snake_body[0].y > FIELD_SIZE - 1) {
                snake_body[0].y = 0;
            }

            if (snake_body[0].x < 0) {
                snake_body[0].x = FIELD_SIZE - 1;
            }

            if (snake_body[0].y < 0) {
                snake_body[0].y = FIELD_SIZE - 1;
            }

            // LOSE
            for (i_coords = 1; i_coords < body_snake_length; i_coords++) {
                if (snake_body[0].x == snake_body[i_coords].x && snake_body[0].y == snake_body[i_coords].y) {
                    game_status = LOSE;

                    text_surf = TTF_RenderText_Solid(font, "You lose!", text_color);
                    text_texture = SDL_CreateTextureFromSurface(renderer, text_surf);

                    SDL_QueryTexture(text_texture, NULL, NULL, &text_pos.w, &text_pos.h);
                    text_pos.x = SCREEN_SIZE[0] / 2 - text_pos.w / 2;
                    text_pos.y = SCREEN_SIZE[1] / 2 - text_pos.h / 2;

                    SDL_SetWindowTitle(window, "AlexSnake - You lose!");
                }
            }

            // if head on eat
            if (snake_body[0].x == eat_spawn_coords.x && snake_body[0].y == eat_spawn_coords.y) {
                map_structure[eat_spawn_coords.x][eat_spawn_coords.y] = 0;

                eat_spawn_coords.x = rand() % 0x20;
                eat_spawn_coords.y = rand() % 0x20;
                body_snake_length++;

                sprintf_s(str_buffer, STR_BUFF_SIZE, "AlexSnake %d/%d & FPS = %d", body_snake_length, SNAKE_MAX_LENGTH, fps);

                SDL_SetWindowTitle(window, str_buffer);

                if (body_snake_length % 5 == 0) game_speed -= 2;
            }

            // WIN
            if (body_snake_length == SNAKE_MAX_LENGTH-1) {
                game_status = WIN;

                text_surf = TTF_RenderText_Solid(font, "You win!", text_color);
                text_texture = SDL_CreateTextureFromSurface(renderer, text_surf);

                SDL_QueryTexture(text_texture, NULL, NULL, &text_pos.w, &text_pos.h);
                text_pos.x = SCREEN_SIZE[0] / 2 - text_pos.w / 2;
                text_pos.y = SCREEN_SIZE[1] / 2 - text_pos.h / 2;

                SDL_SetWindowTitle(window, "AlexSnake - You win!");
            }

            snake_body[rewrite_snake_coords].x = snake_body[0].x;
            snake_body[rewrite_snake_coords].y = snake_body[0].y;


            if (rewrite_snake_coords <= body_snake_length) rewrite_snake_coords++;
            else {
                rewrite_snake_coords = 1;
                is_max_length = SDL_TRUE;
            }

            if (is_max_length == SDL_TRUE) {
                map_structure[snake_body[rewrite_snake_coords].x][snake_body[rewrite_snake_coords].y] = 0;
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

#ifdef GRID_ON
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
                    grid_rect.x = i_coords * BLOCK_SIZE;
                    grid_rect.y = j_coords * BLOCK_SIZE;
                    grid_rect.h = 100;
                    grid_rect.w = 1;
                    SDL_RenderFillRect(renderer, &grid_rect);

                    grid_rect.h = 1;
                    grid_rect.w = 100;
                    SDL_RenderFillRect(renderer, &grid_rect);
#endif
                }
            }

            // after rendering the head, you need to make it part of the body
            map_structure[snake_body[0].x][snake_body[0].y] = 1;
        }
        else if (game_status == WIN) {
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);

            SDL_RenderCopy(renderer, text_texture, NULL, &text_pos);
        }
        else if (game_status == LOSE) {
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);

            SDL_RenderCopy(renderer, text_texture, NULL, &text_pos);
        }

        SDL_RenderPresent(renderer);

        timer_end = SDL_GetTicks();
        if (timer_end - timer_start > 1000) {

            timer_one_tick = (timer_end - timer_start) / 1000;
            timer_start = SDL_GetTicks();

            sprintf_s(str_buffer, STR_BUFF_SIZE, "AlexSnake %d/%d & FPS = %d", body_snake_length, SNAKE_MAX_LENGTH, fps);
            SDL_SetWindowTitle(window, str_buffer);

            fps = 0;
        }
        else fps++;

        if (timer_one_tick < delay_time) {
            SDL_Delay(delay_time - timer_one_tick);
        }
    }

    destroy_resources();
    return 0;
}

int init_resources(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Error", SDL_GetError(), NULL);
        return 1;
    }

    window = SDL_CreateWindow("AlexSnake - simple snake game written in C with SDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_SIZE[0], SCREEN_SIZE[1],
        SDL_WINDOW_SHOWN);

    if (window == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Error", SDL_GetError(), NULL);
        return 2;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Error", SDL_GetError(), NULL);
        return 3;
    }

    if (TTF_Init() != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Error", TTF_GetError(), NULL);
        return 4;
    }
    if (!(font = TTF_OpenFont("arial.ttf", 28))) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Error", TTF_GetError(), NULL);
        return 5;
    }

    return 0;
}

void destroy_resources(void) {
    if (window != NULL) SDL_DestroyWindow(window);
    if (renderer!= NULL) SDL_DestroyRenderer(renderer);

    if (text_surf!= NULL) SDL_FreeSurface(text_surf);
    if (text_texture != NULL) SDL_DestroyTexture(text_texture);
    if (font!= NULL) TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();
}