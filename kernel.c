#include "kernel.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short*)0xB8000)
#define VGA_13H ((unsigned char*)0xA0000)
#define SCREEN_W 320
#define SCREEN_H 200

#define PADDLE_W 6
#define PADDLE_H 30

#define BALL_SIZE 4
void drawRect(int startX, int startY,
              int width, int height,
              unsigned char *vga,
              int color);


//Width  = 320 pixels  → x = 0 .. 319
//Height = 200 pixels  → y = 0 .. 199
void delay() {

    //LOL 
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);
    for (volatile int i = 0; i < 200000; i++);


    
}
unsigned char keyboard_read_scancode() {
    if (inb(0x64) & 1) {      // data available?
        return inb(0x60);    // read scancode
    }
    return 0;
}
int paddle_left_y  = 80;
int paddle_right_y = 80;

int ball_x = 160;
int ball_y = 100;

int ball_vx = 1;   
int ball_vy = 1;
int key_w = 0;
int key_s = 0;

unsigned char sc;
void kmain() {
    unsigned char* vga = VGA_13H;
    while (1) {
        handle_input();
        ai_move();
        update_ball();
        render(vga);

        delay(); 
    }
}
void clear_vga(unsigned char *vga) {
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
        vga[i] = 0; 
}
void handle_input() {
    unsigned char sc = keyboard_read_scancode();
    if (!sc) return;

    if (sc == 0x11) key_w = 1;        
    if (sc == 0x91) key_w = 0;        

    if (sc == 0x1F) key_s = 1;       
    if (sc == 0x9F) key_s = 0;        


}

void ai_move() {
    if (key_w && paddle_left_y > 0)//W
        paddle_left_y -= 1;

    if (key_s && paddle_left_y < SCREEN_H - PADDLE_H)//S
        paddle_left_y += 1;
    if (ball_y < paddle_right_y)
        paddle_right_y -= 2;
    else if (ball_y > paddle_right_y + PADDLE_H)
        paddle_right_y += 2;

    if (paddle_right_y < 0)
        paddle_right_y = 0;
    if (paddle_right_y > SCREEN_H - PADDLE_H)
        paddle_right_y = SCREEN_H - PADDLE_H;
}

void render(unsigned char *vga) {
    clear_vga(vga);


    drawRect(10, paddle_left_y, PADDLE_W, PADDLE_H, vga, 15);

   
    drawRect(SCREEN_W - 10 - PADDLE_W,
             paddle_right_y,
             PADDLE_W, PADDLE_H,
             vga, 15);

    
    drawRect(ball_x, ball_y, BALL_SIZE, BALL_SIZE, vga, 15);
}
void update_ball() {
    ball_x += ball_vx;
    ball_y += ball_vy;

    
    if (ball_y <= 0 || ball_y >= SCREEN_H - BALL_SIZE)
        ball_vy = -ball_vy;

    
    if (ball_x <= 10 + PADDLE_W &&
        ball_y + BALL_SIZE >= paddle_left_y &&
        ball_y <= paddle_left_y + PADDLE_H) {
        ball_vx = -ball_vx;
    }

    
    if (ball_x + BALL_SIZE >= SCREEN_W - 10 - PADDLE_W &&
        ball_y + BALL_SIZE >= paddle_right_y &&
        ball_y <= paddle_right_y + PADDLE_H) {
        ball_vx = -ball_vx;
    }

    
    if (ball_x < 0 || ball_x > SCREEN_W) {
        ball_x = 160;
        ball_y = 100;
        ball_vx = -ball_vx;
    }
}
void drawRect(int startX, int startY,
              int width, int height,
              unsigned char *vga,
              int color)
{
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    int endX = startX + width;
    int endY = startY + height;

    if (endX > SCREEN_W) endX = SCREEN_W;
    if (endY > SCREEN_H) endY = SCREEN_H;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            vga[y * SCREEN_W + x] = color;
        }
    }
}

