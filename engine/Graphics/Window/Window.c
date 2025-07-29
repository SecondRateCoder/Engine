typedef struct{
#ifdef _WIN32 // note the underscore: without it, it's not msdn official!
    // Windows (x64 and x86)
#elif __unix__ // all unices, not all compilers
    // Unix
#elif __linux__
    // linux
#elif __APPLE__
    // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif

// #ifdef WIN32 || __MINGW32__ || __MINGW64__

// #elif __LINUX__

// #elif __APPLE__

// #endif
}