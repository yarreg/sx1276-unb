#include "delay.h"

#if defined(LINUX)
#include <unistd.h>
#elif defined(WINDOWS)
#include <windows.h>
#elif defined(USE_HAL_DRIVER)
    extern void HAL_Delay(uint32_t Delay);
#endif



// delay (in milliseconds) 
void DelayMs(uint32_t ms) {
#if defined(LINUX)
    usleep(ms * 1000);
#elif defined(WINDOWS)
    Sleep(ms);
#elif defined(USE_HAL_DRIVER)
    HAL_Delay(ms);
#else
    #error "Not supported platform"
#endif
}

// Delay in seconds
void Delay(uint32_t sec) {
    DelayMs(sec * 1000);
}