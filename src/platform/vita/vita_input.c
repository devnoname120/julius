#include "vita_input.h"
#include "vita_touch.h"
#include <math.h>

#include "vita_keyboard.h"
#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

int last_mouse_x = 0;
int last_mouse_y = 0;

static SDL_Joystick *joy = NULL;

static int hires_dx = 0;
static int hires_dy = 0;
static int left_pressed;
static int right_pressed;
static int up_pressed;
static int down_pressed;
static int vkbd_requested;

static void vita_start_text_input(char *initial_text, int multiline);
static void rescale_analog(int *x, int *y, int dead);

int vita_poll_event(SDL_Event *event)
{
    int ret = SDL_PollEvent(event);
    if(event != NULL) {
        vita_handle_touch(event);
        switch (event->type) {
            case SDL_MOUSEMOTION:
                // update joystick / touch mouse coords
                last_mouse_x = event->motion.x;
                last_mouse_y = event->motion.y;
                break;
            case SDL_JOYBUTTONDOWN:
                if (event->jbutton.which==0) { // Only Joystick 0 controls the mouse
                    switch (event->jbutton.button) {
                        case PAD_SQUARE:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_PAGEDOWN;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            break;
                        case PAD_TRIANGLE:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_PAGEUP;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            break;
                        case PAD_CROSS:
                        case PAD_R: // intentional fallthrough
                            event->type = SDL_MOUSEBUTTONDOWN;
                            event->button.button = SDL_BUTTON_LEFT;
                            event->button.state = SDL_PRESSED;
                            event->button.x = last_mouse_x;
                            event->button.y = last_mouse_y;
                            break;
                        case PAD_CIRCLE:
                        case PAD_L: // intentional fallthrough
                            event->type = SDL_MOUSEBUTTONDOWN;
                            event->button.button = SDL_BUTTON_RIGHT;
                            event->button.state = SDL_PRESSED;
                            event->button.x = last_mouse_x;
                            event->button.y = last_mouse_y;
                            break;
                        case PAD_UP:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_UP;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            up_pressed = 1;
                            break;
                        case PAD_DOWN:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_DOWN;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            down_pressed = 1;
                            break;
                        case PAD_LEFT:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_LEFT;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            left_pressed = 1;
                            break;
                        case PAD_RIGHT:
                            event->type = SDL_KEYDOWN;
                            event->key.keysym.sym = SDLK_RIGHT;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            right_pressed = 1;
                            break;
                        case PAD_START:
                            vkbd_requested = 1;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case SDL_JOYBUTTONUP:
                if (event->jbutton.which==0) {// Only Joystick 0 controls the mouse
                    switch (event->jbutton.button) {
                        case PAD_SQUARE:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_PAGEDOWN;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            break;
                        case PAD_TRIANGLE:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_PAGEUP;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            break;
                        case PAD_CROSS:
                        case PAD_R: // intentional fallthrough
                            event->type = SDL_MOUSEBUTTONUP;
                            event->button.button = SDL_BUTTON_LEFT;
                            event->button.state = SDL_RELEASED;
                            event->button.x = last_mouse_x;
                            event->button.y = last_mouse_y;
                            break;
                        case PAD_CIRCLE:
                        case PAD_L: // intentional fallthrough
                            event->type = SDL_MOUSEBUTTONUP;
                            event->button.button = SDL_BUTTON_RIGHT;
                            event->button.state = SDL_RELEASED;
                            event->button.x = last_mouse_x;
                            event->button.y = last_mouse_y;
                            break;
                        case PAD_UP:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_UP;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            up_pressed = 0;
                            break;
                        case PAD_DOWN:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_DOWN;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            down_pressed = 0;
                            break;
                        case PAD_LEFT:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_LEFT;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            left_pressed = 0;
                            break;
                        case PAD_RIGHT:
                            event->type = SDL_KEYUP;
                            event->key.keysym.sym = SDLK_RIGHT;
                            event->key.keysym.mod = 0;
                            event->key.repeat = 0;
                            right_pressed = 0;
                            break;
                        default:
                            break;
                    }
                }
            default:
                break;
        }
    }
    return ret;
}

void vita_handle_repeat_keys()
{
    if (up_pressed) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_UP;
        SDL_PushEvent(&event);
    } else if (down_pressed) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_DOWN;
        SDL_PushEvent(&event);
    }
    if (left_pressed) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_LEFT;
        SDL_PushEvent(&event);
    } else if (right_pressed) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_RIGHT;
        SDL_PushEvent(&event);
    }
}

void vita_handle_analog_sticks()
{
    if (!joy) {
        joy = SDL_JoystickOpen(0);
    }
    int left_x = SDL_JoystickGetAxis(joy, 0);
    int left_y = SDL_JoystickGetAxis(joy, 1);
    rescale_analog(&left_x, &left_y, 3000);
    hires_dx += left_x;
    hires_dy += left_y;

    const int slowdown = 4096;

    if (hires_dx != 0 || hires_dy != 0) {
        int xrel = hires_dx / slowdown;
        int yrel = hires_dy / slowdown;
        hires_dx %= slowdown;
        hires_dy %= slowdown;
        if (xrel != 0 || yrel !=0) {
            // limit joystick mouse to screen coords, same as physical mouse
            int x = last_mouse_x + xrel;
            int y = last_mouse_y + yrel;
            if (x < 0) {
                x = 0;
                xrel = 0 - last_mouse_x;
            }
            if (x > DISPLAY_WIDTH) {
                x = DISPLAY_WIDTH;
                xrel = DISPLAY_WIDTH - last_mouse_x;
            }
            if (y < 0) {
                y = 0;
                yrel = 0 - last_mouse_y;
            }
            if (y > DISPLAY_HEIGHT) {
                y = DISPLAY_HEIGHT;
                yrel = DISPLAY_HEIGHT - last_mouse_y;
            }
            SDL_Event event;
            event.type = SDL_MOUSEMOTION;
            event.motion.x = x;
            event.motion.y = y;
            event.motion.xrel = xrel;
            event.motion.yrel = yrel;
            SDL_PushEvent(&event);
        }
    }

    // map right stick to cursor keys
    float right_x = SDL_JoystickGetAxis(joy, 2);
    float right_y = -1 * SDL_JoystickGetAxis(joy, 3);
    float right_joy_dead_zone_squared = 10240.0*10240.0;
    float slope = 0.414214f; // tangent of 22.5 degrees for size of angular zones

    if ((right_x * right_x + right_y * right_y) > right_joy_dead_zone_squared)
    {
        int up = 0;
        int down = 0;
        int left = 0;
        int right = 0;

        // upper right quadrant
        if (right_y > 0 && right_x > 0)
        {
            if (right_y > slope * right_x)
                up = 1;
            if (right_x > slope * right_y)
                right = 1;
        }
        // upper left quadrant
        else if (right_y > 0 && right_x <= 0)
        {
            if (right_y > slope * (-right_x))
                up = 1;
            if ((-right_x) > slope * right_y)
                left = 1;
        }
        // lower right quadrant
        else if (right_y <= 0 && right_x > 0)
        {
            if ((-right_y) > slope * right_x)
                down = 1;
            if (right_x > slope * (-right_y))
                right = 1;
        }
        // lower left quadrant
        else if (right_y <= 0 && right_x <= 0)
        {
            if ((-right_y) > slope * (-right_x))
                down = 1;
            if ((-right_x) > slope * (-right_y))
                left = 1;
        }

        if (!up_pressed && up) {
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&event);
        } else if (!down_pressed && down) {
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&event);
        }
        if (!left_pressed && left) {
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.keysym.sym = SDLK_LEFT;
            SDL_PushEvent(&event);
        } else if (!right_pressed && right) {
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.keysym.sym = SDLK_RIGHT;
            SDL_PushEvent(&event);
        }
    }
}

void vita_handle_virtual_keyboard()
{
    if (vkbd_requested) {
        vkbd_requested = 0;
        vita_start_text_input("", 0);
    }
}

static void vita_start_text_input(char *initial_text, int multiline)
{
    char *text = vita_keyboard_get("Enter New Text:", initial_text, 600, multiline);
    if (text != NULL)  {
        for (int i = 0; i < 600; i++) {
            SDL_Event down_event;
            down_event.type = SDL_KEYDOWN;
            down_event.key.keysym.sym = SDLK_BACKSPACE;
            down_event.key.keysym.mod = 0;
            SDL_PushEvent(&down_event);
            SDL_Event up_event;
            up_event.type = SDL_KEYUP;
            up_event.key.keysym.sym = SDLK_BACKSPACE;
            up_event.key.keysym.mod = 0;
            SDL_PushEvent(&up_event);
        }
        for (int i = 0; i < 600; i++) {
            SDL_Event down_event;
            down_event.type = SDL_KEYDOWN;
            down_event.key.keysym.sym = SDLK_DELETE;
            down_event.key.keysym.mod = 0;
            SDL_PushEvent(&down_event);
            SDL_Event up_event;
            up_event.type = SDL_KEYUP;
            up_event.key.keysym.sym = SDLK_DELETE;
            up_event.key.keysym.mod = 0;
            SDL_PushEvent(&up_event);
        }
        int i=0;
        while (text[i]!=0 && i<599) {
            SDL_Event textinput_event;
            textinput_event.type = SDL_TEXTINPUT;
            textinput_event.text.text[0] = text[i];
            textinput_event.text.text[1] = 0;
            SDL_PushEvent(&textinput_event);
            i++;
        }
    }
}

void rescale_analog(int *x, int *y, int dead)
{
    //radial and scaled dead_zone
    //http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
    //input and output values go from -32767...+32767;

    //the maximum is adjusted to account for SCE_CTRL_MODE_DIGITALANALOG_WIDE
    //where a reported maximum axis value corresponds to 80% of the full range
    //of motion of the analog stick

    if (dead == 0) return;
    if (dead >= 32767) {
        *x = 0;
        *y = 0;
        return;
    }

    const float max_axis = 32767.0f;
    float analog_x = (float) *x;
    float analog_y = (float) *y;
    float dead_zone = (float) dead;

    float magnitude = sqrtf(analog_x * analog_x + analog_y * analog_y);
    if (magnitude >= dead_zone) {
        //adjust maximum magnitude
        float abs_analog_x = fabs(analog_x);
        float abs_analog_y = fabs(analog_y);
        float max_x;
        float max_y;
        if (abs_analog_x > abs_analog_y) {
            max_x = max_axis;
            max_y = (max_axis * analog_y) / abs_analog_x;
        } else {
            max_x = (max_axis * analog_x) / abs_analog_y;
            max_y = max_axis;
        }
        float maximum = sqrtf(max_x * max_x + max_y * max_y);
        if (maximum > 1.25f * max_axis) maximum = 1.25f * max_axis;
        if (maximum < magnitude) maximum = magnitude;

        // find scaled axis values with magnitudes between zero and maximum
        float scalingFactor = maximum / magnitude * (magnitude - dead_zone) / (maximum - dead_zone);
        analog_x = (analog_x * scalingFactor);
        analog_y = (analog_y * scalingFactor);

        // clamp to ensure results will never exceed the max_axis value
        float clamping_factor = 1.0f;
        abs_analog_x = fabs(analog_x);
        abs_analog_y = fabs(analog_y);
        if (abs_analog_x > max_axis || abs_analog_y > max_axis){
            if (abs_analog_x > abs_analog_y)
                clamping_factor = max_axis / abs_analog_x;
            else
                clamping_factor = max_axis / abs_analog_y;
        }

        *x = (int) (clamping_factor * analog_x);
        *y = (int) (clamping_factor * analog_y);
    } else {
        *x = 0;
        *y = 0;
    }
}
