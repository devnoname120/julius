#ifndef VITA_INPUT_H
#define VITA_INPUT_H

#include <SDL2/SDL.h>

int vita_poll_event(SDL_Event *event);
void vita_handle_analog_sticks(void);
void vita_handle_virtual_keyboard(void);
void vita_handle_repeat_keys(void);

#define PAD_UP 8
#define PAD_DOWN 6
#define PAD_LEFT 7
#define PAD_RIGHT 9
#define PAD_TRIANGLE 0
#define PAD_SQUARE 3
#define PAD_CROSS 2
#define PAD_CIRCLE 1
#define PAD_SELECT 10
#define PAD_START 11
#define PAD_L 4
#define PAD_R 5

#endif /* VITA_INPUT_H */
