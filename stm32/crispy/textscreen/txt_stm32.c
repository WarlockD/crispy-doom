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
// Text mode emulation in SDL
//


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>

#include <txt_stm32.h>

#include "../src/doomkeys.h"

#include "txt_main.h"


// has the hal usart drivers here
#include "main.h"

#if !defined(__cplusplus) || defined(_GCC)
#define inline __inline
#endif



// Fonts:
#if 0
#include "txt_font.h"
#include "txt_largefont.h"
#include "txt_smallfont.h"
#endif
// no need
// Time between character blinks in ms

#define BLINK_PERIOD 250

static unsigned char *screendata;
static int key_mapping = 1;
static UART_HandleTypeDef txt_usart;
static DMA_HandleTypeDef txt_usart1_tx;

static TxtUartEventCallbackFunc event_callback;
static void *event_callback_data;

static int modifier_state[TXT_NUM_MODIFIERS];

struct txt_rect {
	size_t x;
	size_t y;
	size_t width;
	size_t height;
};


static volatile uint8_t dma_tx_buffer[512];
static volatile int screen_updating = 0;
static volatile struct txt_rect screen_rect_queue = { 0,0,0,0 }; // rect being updated




#define ANSI_PUTC(P,C)   do { *P++ = (C); } while(0)
#define ANSI_CSI(P) do { ANSI_PUTC(P,'\027'); ANSI_PUTC(P,'['); } while(0)
#define ANSI_SCP(P) do { ANSI_CSI(P); ANSI_PUTC(P,'s'); } while(0) /* save cursor */
#define ANSI_RCP(P) do { ANSI_CSI(P); ANSI_PUTC(P,'u'); } while(0) /* restore cursor */
#define PUT_NUMBER(P, N) \
	do {\
		int __n = (N);\
		while (__n != 0) {\
			ANSI_PUTC(P,(__n % 10) + '0'); \
			__n = __n/10;\
		}\
	} while(0)

static inline uint8_t* ANSI_POS(uint8_t* p, int x, int y) {
	ANSI_CSI(p);
	PUT_NUMBER(p,x);
	ANSI_PUTC(p,';');
	PUT_NUMBER(p,y);
	ANSI_PUTC(p,'H');
	return p;
}
#define ANSI_SGR(P,N) do { ANSI_CSI(P); PUT_NUMBER(P,N); ANSI_PUTC(P,'m');} while(0) /* restore cursor */


#define BUF_LEFT(P) (size_t)(((size_t)(P))-((size_t)&dma_tx_buffer))

#define GET_FG(X) ((X) & 0x7)
#define GET_BG(X) (((X)>>4) & 0x7)
#define GET_BLINK(X) (((X)>>7) & 0x1)
#define GET_BOLD(X) (((X)>>3) & 0x1)

uint8_t* init_attrib(uint8_t * tx, int attr, int* last) {
	if(last) {
		if(*last == attr) return tx;
		int semi = 0;
		ANSI_CSI(tx);

		if( (GET_FG(attr) != GET_FG(*last))) {
			if(semi) ANSI_PUTC(tx,';'); else semi = 1;
			PUT_NUMBER(tx,GET_FG(attr)+30); // forground color
			semi = 1;
		}
		if( (GET_BG(attr) != GET_BG(*last))) {
			if(semi) ANSI_PUTC(tx,';'); else semi = 1;
			PUT_NUMBER(tx,GET_BG(attr)+40); // background color
		}
		if( (GET_BLINK(attr) != GET_BLINK(*last))) {
			if(semi) ANSI_PUTC(tx,';'); else semi = 1;
			if(GET_BLINK(attr)) PUT_NUMBER(tx,5); else PUT_NUMBER(tx,25); // blink
		}
		if((GET_BOLD(attr) != GET_BOLD(*last))) {
			if(semi) ANSI_PUTC(tx,';'); else semi = 1;
			if(GET_BOLD(attr)) PUT_NUMBER(tx,1); else PUT_NUMBER(tx,21); // bold
		}
		*last = attr;
	} else {
		ANSI_CSI(tx);
		PUT_NUMBER(tx,GET_FG(attr)+30); // forground color
		ANSI_PUTC(tx,';');
		PUT_NUMBER(tx,GET_BG(attr)+30); // background color
		ANSI_PUTC(tx,';');
		if(GET_BLINK(attr)) PUT_NUMBER(tx,5); else PUT_NUMBER(tx,25); // blink
		ANSI_PUTC(tx,';');
		if(GET_BOLD(attr)) PUT_NUMBER(tx,1); else PUT_NUMBER(tx,21); // bold
	}
	ANSI_PUTC(tx,'m');
	return tx;
}

static void updating_screen_pump(volatile struct txt_rect* rect) {
	if(!rect || rect->height ==0) {
		screen_updating = 0;
		return;
	}
	uint8_t* tx = &dma_tx_buffer[0];

	size_t x = rect->x;
	uint8_t* p = &screendata[(rect->y * TXT_SCREEN_W + x) * 2];
	int last_attrib = p[1];
	// set inital settings and first char
	tx = init_attrib(tx, last_attrib,NULL);
	do {
		tx = ANSI_POS(tx,x+1,rect->y+1); // set the line position
		do {
			tx = init_attrib(tx,p[1],&last_attrib); // do attribs
			ANSI_PUTC(tx,p[0]);
			x++; p+=2;
		} while(x < rect->width);
		--rect->y;
		if(--rect->height == 0) break;
		x = rect->x;
		p = &screendata[(rect->y * TXT_SCREEN_W + x) * 2];
	} while((BUF_LEFT(tx)+100) < rect->width);
	if(tx != &dma_tx_buffer[0]) {
		assert(HAL_UART_Transmit_DMA(&txt_usart, (uint8_t*)dma_tx_buffer, BUF_LEFT(tx))== HAL_OK);
		screen_updating = 1;
	} else {
		screen_updating = 0;
	}
}
static void update_screen(const struct txt_rect* rect) {
	while(screen_updating);
	screen_rect_queue = *rect;
	updating_screen_pump(&screen_rect_queue);
}

void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&txt_usart);
}
void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(txt_usart.hdmatx);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle == &txt_usart){
		updating_screen_pump(&screen_rect_queue);
	}
}
void test_uart(const char* message){
	size_t len = strlen(message);
	memcpy(&dma_tx_buffer[0],message,len);
	assert(HAL_UART_Transmit_DMA(&txt_usart, (uint8_t*)dma_tx_buffer, len)== HAL_OK);
	screen_updating=1;
	while(screen_updating);
}
void test_uart_blocking(const char* message){
	size_t len = strlen(message);
	memcpy(&dma_tx_buffer[0],message,len);
	assert(HAL_UART_Transmit(&txt_usart, (uint8_t*)dma_tx_buffer, len, 100000)== HAL_OK);
}
//
// Initialize text mode screen
//
// Returns 1 if successful, 0 if an error occurred
//
static void TXT_USART_Pin_Init() {
	  GPIO_InitTypeDef  GPIO_InitStruct;

	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable GPIO TX/RX clock */
	  USARTx_TX_GPIO_CLK_ENABLE();
	  USARTx_RX_GPIO_CLK_ENABLE();


	  /* Enable USARTx clock */
	  USARTx_CLK_ENABLE();

	  /*##-2- Configure peripheral GPIO ##########################################*/
	  /* UART TX GPIO pin configuration  */
	  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	  //GPIO_InitStruct.Pull      = GPIO_PULLUP;

	  GPIO_InitStruct.Pull      = GPIO_NOPULL;
	  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	  //GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = USARTx_TX_AF;

	  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	  /* UART RX GPIO pin configuration  */
	  GPIO_InitStruct.Pin = USARTx_RX_PIN;
	  GPIO_InitStruct.Alternate = USARTx_RX_AF;

	  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

	  /*##-3- Configure the NVIC for UART ########################################*/
	  /* NVIC for USART */
	  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	  HAL_NVIC_EnableIRQ(USARTx_IRQn);

	  HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 0, 1);
	  HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

}

static void TXT_USART_Pin_DeInit() {
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

int TXT_Init(void)
{
	// inital usart1 using the STM32 hall driver
	// maybe go to openstm latter?
	txt_usart.Instance = USART1;
	//txt_usart.Init.BaudRate = 115200;
	//txt_usart.Init.BaudRate =57600;
	txt_usart.Init.BaudRate =9600;
	txt_usart.Init.WordLength = UART_WORDLENGTH_8B;
	txt_usart.Init.StopBits = UART_STOPBITS_1;
	txt_usart.Init.Parity = UART_PARITY_NONE;
	txt_usart.Init.Mode = UART_MODE_TX_RX;
	txt_usart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	txt_usart.Init.OverSampling = UART_OVERSAMPLING_16;
	txt_usart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	txt_usart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	TXT_USART_Pin_Init();
	assert (HAL_UART_Init(&txt_usart) == HAL_OK);
	// we can use dma to update the screen
  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
	txt_usart1_tx.Instance                 = USARTx_TX_DMA_STREAM;
	txt_usart1_tx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
	txt_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	txt_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	txt_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
	txt_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	txt_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	txt_usart1_tx.Init.Mode                = DMA_NORMAL;
	txt_usart1_tx.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&txt_usart1_tx);
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(&txt_usart, hdmatx, txt_usart1_tx);
    screendata = malloc(TXT_SCREEN_W * TXT_SCREEN_H * 2);
    memset(screendata, 0, TXT_SCREEN_W * TXT_SCREEN_H * 2);
    test_uart_blocking("Usart works!\n");
   // test_uart("Usart works!\n");
    return 1;
}

void TXT_Shutdown(void)
{
    free(screendata);
    screendata = NULL;
    TXT_USART_Pin_DeInit();
}

unsigned char *TXT_GetScreenData(void)
{
    return screendata;
}


static int LimitToRange(int val, int min, int max)
{
    if (val < min)
    {
        return min;
    }
    else if (val > max)
    {
        return max;
    }
    else
    {
        return val;
    }
}

void TXT_UpdateScreenArea(int x, int y, int w, int h)
{
    int x1, y1;
    int x_end;
    int y_end;

    x_end = LimitToRange(x + w, 0, TXT_SCREEN_W);
    y_end = LimitToRange(y + h, 0, TXT_SCREEN_H);
    x = LimitToRange(x, 0, TXT_SCREEN_W);
    y = LimitToRange(y, 0, TXT_SCREEN_H);


    struct txt_rect rect;
    rect.x = x;
    rect.y = y;
    rect.width = (x_end - x);
    rect.height = (y_end - y);
    update_screen(&rect);

}

void TXT_UpdateScreen(void)
{
    TXT_UpdateScreenArea(0, 0, TXT_SCREEN_W, TXT_SCREEN_H);
}

void TXT_GetMousePosition(int *x, int *y)
{
    //SDL_GetMouseState(x, y);
	*x = 0;
	*y = 0;
#if 0
    *x /= font->w;
    *y /= font->h;
#endif
}

//
// Translates the SDL key
//
#if 0
static int TranslateKey(SDL_keysym *sym)
{
    switch(sym->sym)
    {
        case SDLK_LEFT:        return KEY_LEFTARROW;
        case SDLK_RIGHT:       return KEY_RIGHTARROW;
        case SDLK_DOWN:        return KEY_DOWNARROW;
        case SDLK_UP:          return KEY_UPARROW;
        case SDLK_ESCAPE:      return KEY_ESCAPE;
        case SDLK_RETURN:      return KEY_ENTER;
        case SDLK_TAB:         return KEY_TAB;
        case SDLK_F1:          return KEY_F1;
        case SDLK_F2:          return KEY_F2;
        case SDLK_F3:          return KEY_F3;
        case SDLK_F4:          return KEY_F4;
        case SDLK_F5:          return KEY_F5;
        case SDLK_F6:          return KEY_F6;
        case SDLK_F7:          return KEY_F7;
        case SDLK_F8:          return KEY_F8;
        case SDLK_F9:          return KEY_F9;
        case SDLK_F10:         return KEY_F10;
        case SDLK_F11:         return KEY_F11;
        case SDLK_F12:         return KEY_F12;
        case SDLK_PRINT:       return KEY_PRTSCR;

        case SDLK_BACKSPACE:   return KEY_BACKSPACE;
        case SDLK_DELETE:      return KEY_DEL;

        case SDLK_PAUSE:       return KEY_PAUSE;

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
                               return KEY_RSHIFT;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
                               return KEY_RCTRL;

        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_LMETA:
        case SDLK_RMETA:
                               return KEY_RALT;

        case SDLK_CAPSLOCK:    return KEY_CAPSLOCK;
        case SDLK_SCROLLOCK:   return KEY_SCRLCK;

        case SDLK_HOME:        return KEY_HOME;
        case SDLK_INSERT:      return KEY_INS;
        case SDLK_END:         return KEY_END;
        case SDLK_PAGEUP:      return KEY_PGUP;
        case SDLK_PAGEDOWN:    return KEY_PGDN;

#ifdef SDL_HAVE_APP_KEYS
        case SDLK_APP1:        return KEY_F1;
        case SDLK_APP2:        return KEY_F2;
        case SDLK_APP3:        return KEY_F3;
        case SDLK_APP4:        return KEY_F4;
        case SDLK_APP5:        return KEY_F5;
        case SDLK_APP6:        return KEY_F6;
#endif

        default:               break;
    }

    // Returned value is different, depending on whether key mapping is
    // enabled.  Key mapping is preferable most of the time, for typing
    // in text, etc.  However, when we want to read raw keyboard codes
    // for the setup keyboard configuration dialog, we want the raw
    // key code.

    if (key_mapping)
    {
        // Unicode characters beyond the ASCII range need to be
        // mapped up into textscreen's Unicode range.

        if (sym->unicode < 128)
        {
            return sym->unicode;
        }
        else
        {
            return sym->unicode - 128 + TXT_UNICODE_BASE;
        }
    }
    else
    {
        // Keypad mapping is only done when we want a raw value:
        // most of the time, the keypad should behave as it normally
        // does.

        switch (sym->sym)
        {
            case SDLK_KP0:         return KEYP_0;
            case SDLK_KP1:         return KEYP_1;
            case SDLK_KP2:         return KEYP_2;
            case SDLK_KP3:         return KEYP_3;
            case SDLK_KP4:         return KEYP_4;
            case SDLK_KP5:         return KEYP_5;
            case SDLK_KP6:         return KEYP_6;
            case SDLK_KP7:         return KEYP_7;
            case SDLK_KP8:         return KEYP_8;
            case SDLK_KP9:         return KEYP_9;

            case SDLK_KP_PERIOD:   return KEYP_PERIOD;
            case SDLK_KP_MULTIPLY: return KEYP_MULTIPLY;
            case SDLK_KP_PLUS:     return KEYP_PLUS;
            case SDLK_KP_MINUS:    return KEYP_MINUS;
            case SDLK_KP_DIVIDE:   return KEYP_DIVIDE;
            case SDLK_KP_EQUALS:   return KEYP_EQUALS;
            case SDLK_KP_ENTER:    return KEYP_ENTER;

            default:
                return tolower(sym->sym);
        }
    }
}

#endif

static int MouseHasMoved(void)
{
    static int last_x = 0, last_y = 0;
    int x, y;

    TXT_GetMousePosition(&x, &y);

    if (x != last_x || y != last_y)
    {
        last_x = x; last_y = y;
        return 1;
    }
    else
    {
        return 0;
    }
}

signed int TXT_GetChar(void)
{
#if 0
    SDL_Event ev;

    while (SDL_PollEvent(&ev))
    {
        // If there is an event callback, allow it to intercept this
        // event.

        if (event_callback != NULL)
        {
            if (event_callback(&ev, event_callback_data))
            {
                continue;
            }
        }

        // Process the event.

        switch (ev.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                if (ev.button.button < TXT_MAX_MOUSE_BUTTONS)
                {
                    return SDLButtonToTXTButton(ev.button.button);
                }
                break;

            case SDL_KEYDOWN:
                UpdateModifierState(&ev.key.keysym, 1);

                return TranslateKey(&ev.key.keysym);

            case SDL_KEYUP:
                UpdateModifierState(&ev.key.keysym, 0);
                break;

            case SDL_QUIT:
                // Quit = escape
                return 27;

            case SDL_MOUSEMOTION:
                if (MouseHasMoved())
                {
                    return 0;
                }

            default:
                break;
        }
    }
#endif

    return -1;
}

int TXT_GetModifierState(txt_modifier_t mod)
{
    if (mod < TXT_NUM_MODIFIERS)
    {
        return modifier_state[mod] > 0;
    }

    return 0;
}

static const char *SpecialKeyName(int key)
{
    switch (key)
    {
        case ' ':             return "SPACE";
        case KEY_RIGHTARROW:  return "RIGHT";
        case KEY_LEFTARROW:   return "LEFT";
        case KEY_UPARROW:     return "UP";
        case KEY_DOWNARROW:   return "DOWN";
        case KEY_ESCAPE:      return "ESC";
        case KEY_ENTER:       return "ENTER";
        case KEY_TAB:         return "TAB";
        case KEY_F1:          return "F1";
        case KEY_F2:          return "F2";
        case KEY_F3:          return "F3";
        case KEY_F4:          return "F4";
        case KEY_F5:          return "F5";
        case KEY_F6:          return "F6";
        case KEY_F7:          return "F7";
        case KEY_F8:          return "F8";
        case KEY_F9:          return "F9";
        case KEY_F10:         return "F10";
        case KEY_F11:         return "F11";
        case KEY_F12:         return "F12";
        case KEY_BACKSPACE:   return "BKSP";
        case KEY_PAUSE:       return "PAUSE";
        case KEY_EQUALS:      return "EQUALS";
        case KEY_MINUS:       return "MINUS";
        case KEY_RSHIFT:      return "SHIFT";
        case KEY_RCTRL:       return "CTRL";
        case KEY_RALT:        return "ALT";
        case KEY_CAPSLOCK:    return "CAPS";
        case KEY_SCRLCK:      return "SCRLCK";
        case KEY_HOME:        return "HOME";
        case KEY_END:         return "END";
        case KEY_PGUP:        return "PGUP";
        case KEY_PGDN:        return "PGDN";
        case KEY_INS:         return "INS";
        case KEY_DEL:         return "DEL";
        case KEY_PRTSCR:      return "PRTSC";
                 /*
        case KEYP_0:          return "PAD0";
        case KEYP_1:          return "PAD1";
        case KEYP_2:          return "PAD2";
        case KEYP_3:          return "PAD3";
        case KEYP_4:          return "PAD4";
        case KEYP_5:          return "PAD5";
        case KEYP_6:          return "PAD6";
        case KEYP_7:          return "PAD7";
        case KEYP_8:          return "PAD8";
        case KEYP_9:          return "PAD9";
        case KEYP_UPARROW:    return "PAD_U";
        case KEYP_DOWNARROW:  return "PAD_D";
        case KEYP_LEFTARROW:  return "PAD_L";
        case KEYP_RIGHTARROW: return "PAD_R";
        case KEYP_MULTIPLY:   return "PAD*";
        case KEYP_PLUS:       return "PAD+";
        case KEYP_MINUS:      return "PAD-";
        case KEYP_DIVIDE:     return "PAD/";
                   */

        default:              return NULL;
    }
}

void TXT_GetKeyDescription(int key, char *buf, size_t buf_len)
{
    const char *keyname;

    keyname = SpecialKeyName(key);

    if (keyname != NULL)
    {
        TXT_StringCopy(buf, keyname, buf_len);
    }
    else if (isprint(key))
    {
        TXT_snprintf(buf, buf_len, "%c", toupper(key));
    }
    else
    {
        TXT_snprintf(buf, buf_len, "??%i", key);
    }
}

// Searches the desktop screen buffer to determine whether there are any
// blinking characters.

int TXT_ScreenHasBlinkingChars(void)
{
    int x, y;
    unsigned char *p;

    // Check all characters in screen buffer

    for (y=0; y<TXT_SCREEN_H; ++y)
    {
        for (x=0; x<TXT_SCREEN_W; ++x) 
        {
            p = &screendata[(y * TXT_SCREEN_W + x) * 2];

            if (p[1] & 0x80)
            {
                // This character is blinking

                return 1;
            }
        }
    }

    // None found

    return 0;
}

// Sleeps until an event is received, the screen needs to be redrawn, 
// or until timeout expires (if timeout != 0)

void TXT_Sleep(int timeout)
{
    unsigned int start_time;

    if (timeout == 0)
    {
        // We can just wait forever until an event occurs
    	while(1) {};

    }
    else
    {
    	HAL_Delay(timeout);
        // Sit in a busy loop until the timeout expires or we have to
    }
}

void TXT_EnableKeyMapping(int enable)
{
    key_mapping = enable;
}

void TXT_SetWindowTitle(char *title)
{
	fprintf(stderr,"Setting text Window title: %s\n", title);
}


// Set a callback function to call in the SDL event loop.  Useful for
// intercepting events.  Pass callback=NULL to clear an existing
// callback function.
// user_data is a void pointer to be passed to the callback function.

void TXT_UART_SetEventCallback(TxtUartEventCallbackFunc callback, void *user_data)
{
    event_callback = callback;
    event_callback_data = user_data;
}

// Safe string functions.

void TXT_StringCopy(char *dest, const char *src, size_t dest_len)
{
    if (dest_len < 1)
    {
        return;
    }

    dest[dest_len - 1] = '\0';
    strncpy(dest, src, dest_len - 1);
}

void TXT_StringConcat(char *dest, const char *src, size_t dest_len)
{
    size_t offset;

    offset = strlen(dest);
    if (offset > dest_len)
    {
        offset = dest_len;
    }

    TXT_StringCopy(dest + offset, src, dest_len - offset);
}

// On Windows, vsnprintf() is _vsnprintf().
#ifdef _WIN32
#if _MSC_VER < 1400 /* not needed for Visual Studio 2008 */
#define vsnprintf _vsnprintf
#endif
#endif

// Safe, portable vsnprintf().
int TXT_vsnprintf(char *buf, size_t buf_len, const char *s, va_list args)
{
    int result;

    if (buf_len < 1)
    {
        return 0;
    }

    // Windows (and other OSes?) has a vsnprintf() that doesn't always
    // append a trailing \0. So we must do it, and write into a buffer
    // that is one byte shorter; otherwise this function is unsafe.
    result = vsnprintf(buf, buf_len, s, args);

    // If truncated, change the final char in the buffer to a \0.
    // A negative result indicates a truncated buffer on Windows.
    if (result < 0 || (size_t)result >= buf_len)
    {
        buf[buf_len - 1] = '\0';
        result = buf_len - 1;
    }

    return result;
}

// Safe, portable snprintf().
int TXT_snprintf(char *buf, size_t buf_len, const char *s, ...)
{
    va_list args;
    int result;
    va_start(args, s);
    result = TXT_vsnprintf(buf, buf_len, s, args);
    va_end(args);
    return result;
}

