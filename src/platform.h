#pragma once

#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    #define PLATFORM_WIN 1
#else
    #define PLATFORM_WIN 0
#endif
