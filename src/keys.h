#pragma once

#include "platform.h"

#if PLATFORM_WIN == 1

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

#define HOME 71
#define END 79

#define ENTER 13
#define ESCAPE 27
#define SPACE_BAR 32
#define BACK_SPACE 8
#define DELETE 83

#else

#define LEFT 68
#define RIGHT 67
#define UP 65
#define DOWN 66

#define HOME 72
#define END 70

#define ENTER 10
#define ESCAPE 27
#define SPACE_BAR 32
#define BACK_SPACE 127
#define DELETE 126


#endif


char readkey(); // keys.c
