#ifndef PLATFORM_VITA_H
#define PLATFORM_VITA_H

#define VITA_PATH_PREFIX "ux0:/data/julius/"
#define VITA_DISPLAY_WIDTH 960
#define VITA_DISPLAY_HEIGHT 544

int chdir(const char *path);

char* vita_prepend_path(const char *path);

#endif // PLATFORM_VITA_H