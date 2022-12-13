# AlexSnake
AlexSnake - simple snake game written in C with SDL


![alt text](https://github.com/a1excoder/AlexSnake/blob/main/bad_demo.gif?raw=true)

### A lot of commented code because maybe it will be useful to me in the future.

### I was too lazy to connect SDL_TTF, so when you need to notify the player about the event, a MessageBox is displayed.
```c

    // eat yourself | you lose
    for (i_coords = 1; i_coords < body_snake_length; i_coords++) {
        if (snake_body[0].x == snake_body[i_coords].x && snake_body[0].y == snake_body[i_coords].y) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "You lose", "You lose!", window);
            quit = SDL_FALSE;
        }
    }

```

```c
    // snake length == max(32) then game stop
    if (body_snake_length == SNAKE_MAX_LENGTH) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "You win", "You win!", window);
        quit = SDL_FALSE;
    }
```

### and...
```c
        if (is_max_length == SDL_TRUE) {
            // sometimes in snake_body[rewrite_snake_coords] x and y values > max field size or < min size
            // this crutch need for fix this bug
            // I'm actually just lazy and don't know how to use a debugger ;)
            if (snake_body[rewrite_snake_coords].x < FIELD_SIZE && snake_body[rewrite_snake_coords].x >= 0 &&
                    snake_body[rewrite_snake_coords].y < FIELD_SIZE && snake_body[rewrite_snake_coords].y >= 0) {
                map_structure[snake_body[rewrite_snake_coords].x][snake_body[rewrite_snake_coords].y] = 0;
            }
        }
```
