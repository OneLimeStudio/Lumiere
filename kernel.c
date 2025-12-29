#include "kernel.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short*)0xB8000)

void clear_screen(void) {
    unsigned short blank = (0x07 << 8) | ' '; // light grey on black

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = blank;
    }
}

void kmain() {
    clear_screen();
    char* video_memory = (char*) 0xB8000;
    
    // Write a '!' in Red to prove we arrived
    video_memory[160] = '!';
    video_memory[161] = 0x0C; 

    while(1); // Stop here! If it still restarts, the jump failed.
}