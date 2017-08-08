//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// Text mode I/O functions, similar to C stdio
//

#include <stdlib.h>
#include <string.h>

#include "txt_io.h"
#include "txt_main.h"

static int cur_x = 0, cur_y = 0;
static txt_color_t fgcolor = TXT_COLOR_GREY;
static txt_color_t bgcolor = TXT_COLOR_BLACK;
void uart_putc(int c);
void uart_puts(const char* str);
void uart_write(const uint8_t* data,size_t len);

void usart_gotoxy(int x, int y);
//#define ANSI_TERMINAL

void TXT_GotoXY(int x, int y)
{
#ifdef ANSI_TERMINAL
	if(x != cur_x || y != cur_y){
		usart_gotoxy(x,y);
	}
#endif
    cur_x = x;
    cur_y = y;
}

void TXT_GetXY(int *x, int *y)
{
    *x = cur_x;
    *y = cur_y;
}

static void NewLine(unsigned char *screendata)
{
    int i;
    unsigned char *p;

    cur_x = 0;
    ++cur_y;

    if (cur_y >= TXT_SCREEN_H)
    {
        // Scroll the screen up

        cur_y = TXT_SCREEN_H - 1;

        memmove(screendata, screendata + TXT_SCREEN_W * 2,
                TXT_SCREEN_W * 2 * (TXT_SCREEN_H -1));

        // Clear the bottom line

        p = screendata + (TXT_SCREEN_H - 1) * 2 * TXT_SCREEN_W;

        for (i=0; i<TXT_SCREEN_W; ++i) 
        {
            *p++ = ' ';
            *p++ = fgcolor | (bgcolor << 4);
        }
    }
}

static void PutChar(unsigned char *screendata, int c)
{


#ifdef ANSI_TERMINAL
	int x,y;
	TXT_GetXY(&x,&y);
    // Add a new character to the buffer
    switch (c)
    {
    case '\n':
    	uart_putc('\r');
    	uart_putc('\n');
        x= 0;
        ++y;
        if (y >= TXT_SCREEN_H) y = TXT_SCREEN_H - 1;
    	return;
    case '\b':
        // backspace
        --x;
        if (x < 0) x = 0;
        break;
    default:
        // Add a new character to the buffer
       // p[0] = c;
      //  p[1] = fgcolor | (bgcolor << 4);
    	uart_putc(c);
    	++x;
    	if (x >= TXT_SCREEN_W) {
        	uart_putc('\r');
        	uart_putc('\n');
            x= 0;
            ++y;
            if (y >= TXT_SCREEN_H) y = TXT_SCREEN_H - 1;
    	}
    }
    TXT_GotoXY(x,y);

#else
    unsigned char *p;

    p = screendata + cur_y * TXT_SCREEN_W * 2 +  cur_x * 2;
    switch (c)
    {
        case '\n':
             NewLine(screendata);
            break;

        case '\b':
            // backspace
            --cur_x;
            if (cur_x < 0)
                cur_x = 0;
            break;

        default:

            // Add a new character to the buffer

            p[0] = c;
            p[1] = fgcolor | (bgcolor << 4);

            ++cur_x;

            if (cur_x >= TXT_SCREEN_W) 
            {
                NewLine(screendata);
            }

            break;

    }
#endif
}

void TXT_PutChar(int c)
{

    unsigned char *screen;

    screen = TXT_GetScreenData();

    PutChar(screen, c);
}

void TXT_Puts(const char *s)
{
    unsigned char *screen;
    const char *p;

    screen = TXT_GetScreenData();

    for (p=s; *p != '\0'; ++p)
    {
        PutChar(screen, *p);
    }

    PutChar(screen, '\n');
}




void TXT_FGColor(txt_color_t color)
{

    fgcolor = color;
}

void TXT_BGColor(int color, int blinking)
{

    bgcolor = color;
    if (blinking)
        bgcolor |= TXT_COLOR_BLINKING;
}

void TXT_SaveColors(txt_saved_colors_t *save)
{
    save->bgcolor = bgcolor;
    save->fgcolor = fgcolor;
}

void TXT_RestoreColors(txt_saved_colors_t *save)
{
	bgcolor = save->bgcolor;
	fgcolor = save->fgcolor;
}

void TXT_ClearScreen(void)
{
#ifdef ANSI_TERMINAL
    uart_puts("\033c");	  // reset term
    uart_puts("\033[2J\033[;H");   // clear the screen

    // enable mouse
    uart_puts("\033[?1000h"); // enable mouse tracking?
    uart_puts("\033+C"); // enable mouse tracking?
#endif
    unsigned char *screen;
    int i;

    screen = TXT_GetScreenData();

    for (i=0; i<TXT_SCREEN_W * TXT_SCREEN_H; ++i)
    {
        screen[i * 2] = ' ';
        screen[i * 2 +  1] = (bgcolor << 4) | fgcolor;
    }

    cur_x = 0;
    cur_y = 0;
}

