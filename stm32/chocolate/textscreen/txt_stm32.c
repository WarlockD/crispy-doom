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
#include <stdbool.h>


#include "txt_stm32.h"

#include "doomkeys.h"

#include "txt_main.h"

//#define DEBUG_ANSI
// has the hal usart drivers here
#include "main.h"

#define BLOCK_SEND
#if !defined(__cplusplus) || defined(_GCC)
#define inline __inline
#endif

static void debug_char(const char* msg, int c) {
	if(isprint(c))
		fprintf(stderr, "%s '%c'(%u)\n", msg, (char)c,c);
	else
		fprintf(stderr, "%s <%2.2X>(%u)\n", msg, (char)c,c);
}



//static unsigned char *screendata;

static unsigned char prev_screendata[TXT_SCREEN_W * TXT_SCREEN_H * 2];
static unsigned char screendata[TXT_SCREEN_W * TXT_SCREEN_H * 2];
static int key_mapping = 1;
static UART_HandleTypeDef txt_usart;
static DMA_HandleTypeDef txt_usart1_tx;
#if 0
static inline bool uart_busy() {
	HAL_UART_StateTypeDef state = HAL_UART_GetState(&txt_usart);
	assert(HAL_UART_STATE_RESET != state);
	assert(HAL_UART_STATE_TIMEOUT != state);
	assert(HAL_UART_STATE_ERROR != state);
	if(state  == HAL_UART_STATE_READY) return false;
	else {
#if 0
		  HAL_UART_STATE_BUSY              = 0x24U,   /*!< an internal process is ongoing
		                                                   Value is allowed for gState only */
		  HAL_UART_STATE_BUSY_TX           = 0x21U,   /*!< Data Transmission process is ongoing
		                                                   Value is allowed for gState only */
		  HAL_UART_STATE_BUSY_RX           = 0x22U,   /*!< Data Reception process is ongoing
		                                                   Value is allowed for RxState only */
		  HAL_UART_STATE_BUSY_TX_RX        = 0x23U,   /*!< Data Transmission and Reception process is ongoing */
#endif
		return true;
	}
}
#endif
static inline bool uart_busy(){

	uint32_t isrflags = READ_REG(USART1->ISR);
	return (isrflags & (USART_ISR_RXNE|USART_ISR_TC)) != RESET;

}
inline static void screen_wait() {
	//while(screen_state.updating || uart_busy()) __WFI();
	while(uart_busy());
}

static TxtUartEventCallbackFunc event_callback;
static void *event_callback_data;

static int modifier_state[TXT_NUM_MODIFIERS];

struct txt_rect {
	size_t x;
	size_t y;
	size_t width;
	size_t height;
};


static uint8_t dma_tx_buffer[512];
static uint8_t dma_rx_buffer[64];
typedef struct {
	uint8_t* ptr;
	size_t size;
	size_t pos;
}buffer_t;

typedef struct {
	uint8_t* ptr;
	size_t size;
	size_t head;
	size_t tail;
}circle_t;

inline static void circle_clear(circle_t* circle){
	circle->head = circle->tail = 0;
}
inline static void circle_init(circle_t* circle, uint8_t* ptr, size_t size){
	circle->ptr = ptr;
	circle->size  = size;
	circle_clear(circle);
}
inline static bool circle_empty(circle_t* circle) { return circle->head == circle->tail; }
inline static bool circle_putc(circle_t* circle, uint8_t c){
	volatile size_t head = circle->head;
	circle->ptr[head] = c;
	if(++head > circle->size) head = 0;
	circle->head = head;
	return circle->head == circle->tail;
}

inline static size_t circle_puts(circle_t* circle, const char* str) {
	size_t cnt=0;
	while(*str) { if(circle_putc(circle,*str++)) break;++cnt; }
	return cnt;
}
inline static size_t circle_write(circle_t* circle, const uint8_t* data,size_t len) {
	size_t cnt=0;
	while(len--) { if(circle_putc(circle,*data++)) break;++cnt; }
	return cnt;
}
inline static size_t circle_number(circle_t* circle, int num) {
	char nbuf[9];
	return circle_puts(circle,itoa(num,nbuf,10));
}

inline static bool circle_getc(circle_t* circle, uint8_t* c){
	if(circle_empty(circle)) return false;
	volatile size_t tail = circle->tail;
	*c = circle->ptr[tail];
	if(++tail > circle->size) tail = 0;
	circle->tail = tail;
	return true;
}
inline static void buffer_init(buffer_t* buffer, uint8_t* ptr, size_t size){
	buffer->ptr = ptr;
	buffer->size  = size;
	buffer->pos = 0;
}
inline static void buffer_clear(buffer_t* buffer) {
	buffer->pos = 0;
}
inline static void buffer_putc(buffer_t* buffer, uint8_t c) {
	assert(buffer->size > buffer->pos);
	buffer->ptr[buffer->pos++] = c;
}

inline static void buffer_puts(buffer_t* buffer,const char* str) {
	while(*str) buffer_putc(buffer,*str++);
}
inline static void buffer_write(buffer_t* buffer,const uint8_t* data,size_t len) {
	while(len--) buffer_putc(buffer,*data++);
}
inline static void buffer_number(buffer_t* buffer, int num) {
	char nbuf[8];
	buffer_puts(buffer,itoa(num,nbuf,10));
#if 0
	// stupid simple
	if(num == 0) buffer_putc(buffer,'0');
	if(num > 999) {
		buffer_putc(buffer,num%1000 + '0');
		num/=1000;
	}
	if(num > 99) {
		buffer_putc(buffer,num%100 + '0');
		num/=100;
	}
	if(num > 9) {
		buffer_putc(buffer,num%10+ '0');
		num/=10;
	}
	if(num > 0){
		buffer_putc(buffer,num+ '0');
	}
#endif
}
typedef struct  {
	uint8_t begin_x;
	uint8_t end_x;
} line_info_t;
typedef struct screen_state_s {
	int updating;
	struct txt_rect rect;
	circle_t tx_buffer;
	circle_t rx_buffer;
	size_t cur_y;
	size_t end_y;
	int last_attrib;
	line_info_t lines[TXT_SCREEN_H];
	void (*update)();
}screen_state_t;

static struct screen_state_s screen_state;

void hal_assert(HAL_StatusTypeDef status) {
	if(status == HAL_OK)return;
	switch(status){
	case  HAL_ERROR:
		fprintf(stderr,"HAL ERROR");
		break;
	case HAL_BUSY:
		fprintf(stderr,"HAL BUSY");
		break;
	case  HAL_TIMEOUT:
		fprintf(stderr,"HAL TIMEOUT");
		break;
	default:
		fprintf(stderr,"HAL UNKNOWN");
		break;
	}
	fprintf(stderr," (%u)\n\n",status);
	assert(0);

}
void buffer_trasmit(const buffer_t* buffer) {
	screen_wait();


#ifdef BLOCK_SEND
	hal_assert(HAL_UART_Transmit(&txt_usart, buffer->ptr, buffer->pos, 500));
#else
	hal_assert(HAL_UART_Transmit_DMA(&txt_usart, buffer->ptr, buffer->pos));
#endif
}

void trasmit_buffer(){
	screen_wait();
	uint8_t c;
	if(circle_getc(&screen_state.tx_buffer,&c)){
		//txt_usart.Instance->TDR = c;
		SET_BIT(txt_usart.Instance->CR1, USART_CR1_TXEIE);
	}
}
void send_uart_blocking(const char* message){
	circle_clear(&screen_state.tx_buffer);
	circle_puts(&screen_state.tx_buffer,message);
	trasmit_buffer();
}
void uart_putc(int c) {
	while((USART1->ISR & USART_ISR_TXE)==0);
	USART1->TDR = (uint8_t)(c & 0xFF);
}
void uart_puts(const char* str){
	while(*str) uart_putc(*str++);
}
void uart_write(const uint8_t* data,size_t len){
	while(len--) uart_putc(*data++);
}
void uart_write_circle(circle_t* circle){
	uint8_t c;
	while(circle_getc(circle,&c))
		uart_putc(c);
}
void usart_gotoxy(int x, int y) {
	char num[9];
	uart_putc(27);
	uart_putc('[');
	uart_puts(itoa(y+1,num,10)+1);
	uart_putc(';');
	uart_puts(itoa(y+1,num,10)+1);
	uart_putc('H');
}

#define GET_SCREEN_BUFFER (&screen_state.tx_buffer)
#define ANSI_PUTC(C) do { circle_putc(GET_SCREEN_BUFFER,(C)); } while(0) /* save cursor */
#define PUT_NUMBER(N) do { circle_number(GET_SCREEN_BUFFER,(N)); } while(0) /* save cursor */

#ifdef DEBUG_ANSI
#define ANSI_CSI() do { ANSI_PUTC('\n');  ANSI_PUTC('\r'); ANSI_PUTC(':'); ANSI_PUTC('['); } while(0)
#else
#define ANSI_CSI() do { ANSI_PUTC(27); ANSI_PUTC('['); } while(0)
#endif


#define ANSI_SCP() do { ANSI_CSI(); ANSI_PUTC('s'); } while(0) /* save cursor */
#define ANSI_RCP() do { ANSI_CSI(); ANSI_PUTC('u'); } while(0) /* restore cursor */

#define BUF_LEFT(P) (size_t)(((size_t)(P))-((size_t)&dma_tx_buffer))

#define GET_FG(X) ((X) & 0x7)
#define GET_BG(X) (((X)>>4) & 0x7)
#define GET_BLINK(X) (((X)&0x80) >> 7)
#define GET_BOLD(X) (((X)&0x08) >>3)


static void ANSI_POS(int x, int y) {
	assert(x < TXT_SCREEN_W);
	assert(y < TXT_SCREEN_H);
	ANSI_CSI();
	char nbuf[15];
	circle_puts(&screen_state.tx_buffer,itoa(y+1,nbuf,10));
	//PUT_NUMBER(y+1);
	ANSI_PUTC(';');
	//PUT_NUMBER(x+1);
	circle_puts(&screen_state.tx_buffer,itoa(x+1,nbuf,10));
	ANSI_PUTC('H');
}


#if 0
void position_report(size_t* x, size_t* y) {
	screen_buffer_clear();
	screen_buffer_puts("\033[6n");
}
NCURSES_SP_NAME(_nc_flush) (NCURSES_SP_ARG);
memset(buf, '\0', sizeof(buf));
NCURSES_PUTP2_FLUSH("cpr", "\033[6n");	/* only works on ANSI-compatibles */
*(s = buf) = 0;
do {
int ask = sizeof(buf) - 1 - (s - buf);
int got = read(0, s, ask);
if (got == 0)
    break;
s += got;
} while (strchr(buf, 'R') == 0);
_tracef("probe returned %s", _nc_visbuf(buf));

/* try to interpret as a position report */
if (sscanf(buf, "\033[%d;%dR", &y, &x) != 2) {
_tracef("position probe failed in %s", legend);
} else {
}
}
#endif

int assert_range(volatile int value, volatile int begin,volatile int end) {
	assert(value >= begin && value <= end);
	return value;
}

static void init_attrib(int attr, int* last) {

//	return tx; // ignore attribs for right now
	if(last && *last !=-1) {
		if(*last == attr) return;
		int semi = 0;
		ANSI_CSI();

		if( (GET_FG(attr) != GET_FG(*last))) {
			assert_range(GET_FG(attr)+30,30,37);
			PUT_NUMBER(GET_FG(attr)+30); // forground color
			semi = 1;
		}
		if( (GET_BG(attr) != GET_BG(*last))) {
			assert_range(GET_BG(attr)+40,40,47);
			if(semi) ANSI_PUTC(';'); else semi = 1;
			PUT_NUMBER(GET_BG(attr)+40); // background color
		}
		if( (GET_BLINK(attr) != GET_BLINK(*last))) {
			if(semi) ANSI_PUTC(';'); else semi = 1;
			if(GET_BLINK(attr)) PUT_NUMBER(5); else PUT_NUMBER(25); // blink
		}
		if((GET_BOLD(attr) != GET_BOLD(*last))) {
			if(semi) ANSI_PUTC(';'); else semi = 1;
			if(GET_BOLD(attr)) PUT_NUMBER(1); else PUT_NUMBER(22); // bold
		}
		*last = attr;
	} else {
		ANSI_CSI();
		PUT_NUMBER(GET_FG(attr)+30); // forground color
		ANSI_PUTC(';');
		PUT_NUMBER(GET_BG(attr)+40); // background color
		ANSI_PUTC(';');
		if(GET_BLINK(attr)) PUT_NUMBER(5); else PUT_NUMBER(25); // blink
		ANSI_PUTC(';');
		if(GET_BOLD(attr)) PUT_NUMBER(1); else PUT_NUMBER(22); // bold
		if(last) *last = attr;
	}
	ANSI_PUTC('m');
}

#include "cp437_to_utf.h"
#if 0
static size_t code_to_utf8(unsigned char *const buffer, const unsigned int code)
{
    if (code <= 0x7F) {
        buffer[0] = code;
        return 1;
    }
    if (code <= 0x7FF) {
        buffer[0] = 0xC0 | (code >> 6);            /* 110xxxxx */
        buffer[1] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 2;
    }
    if (code <= 0xFFFF) {
        buffer[0] = 0xE0 | (code >> 12);           /* 1110xxxx */
        buffer[1] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[2] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 3;
    }
    if (code <= 0x10FFFF) {
        buffer[0] = 0xF0 | (code >> 18);           /* 11110xxx */
        buffer[1] = 0x80 | ((code >> 12) & 0x3F);  /* 10xxxxxx */
        buffer[2] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[3] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 4;
    }
    return 0;
}
#endif
void screen_utf8(const uint32_t code) {

	if(code < 0x80)
		ANSI_PUTC(code);
	else if(code < 0x800) {
		ANSI_PUTC(0xC0 | (code >> 6));
		ANSI_PUTC(0x80 | (code & 0x3F));
	} else if(code<0x1000) {
		ANSI_PUTC(0xE0 | (code >> 12));
		ANSI_PUTC(0x80 | ((code >> 6) & 0x3F));
		ANSI_PUTC(0x80 | (code & 0x3F));
	} else {
		ANSI_PUTC(0xF0 | (code >> 18));
		ANSI_PUTC(0x80 | ((code >> 12) & 0x3F));
		ANSI_PUTC(0x80 | ((code >> 6) & 0x3F));
		ANSI_PUTC(0x80 | (code & 0x3F));
	}
}
extern char *TXT_EncodeUTF8(char *p, unsigned int c);
extern const uint16_t cp437_unicode[];
void vga_to_utf8(int c) {
	circle_putc(GET_SCREEN_BUFFER,c);
#if 0
	char buf[5] = {0,0,0,0};
	TXT_EncodeUTF8(buf,c);
	circle_puts(GET_SCREEN_BUFFER,buf);
#endif
//	int
	//if(c < 32)
//		screen_utf8(vga_cp437_to_utf[c]);
	//else {
//	screen_utf8(cp437_unicode);
		//screen_utf8(cp437_to_utf[c]);
	//}
}
static  int last_attrib=-1;
static void trasform_line(size_t line, size_t first, size_t len, uint16_t* src){
	ANSI_POS(first,line);
	while(len--)	{
		init_attrib(*src >> 8,&last_attrib); // do attribs
		uint8_t c = *src & 0xFF;
		vga_to_utf8(c);
		src++;
	}
}

static bool do_update(size_t line) {
	bool ret = false;
	volatile size_t first = 0;
	size_t last = TXT_SCREEN_W-1;
	size_t len = 0;
	uint16_t* src = (uint16_t*)&screendata[(line * TXT_SCREEN_W)*2];// * 2];
	uint16_t* dest = (uint16_t*)&prev_screendata[(line * TXT_SCREEN_W)*2];// * 2];
    while (first <= last)  {
	  /* build up a run of changed cells; if two runs are
		 separated by a single unchanged cell, ignore the
		 break */
        while ((first + len) <= last &&
               (src[first + len] != dest[first + len] ||
                (len && first + len < last &&
                 src[first + len + 1] != dest[first + len + 1])
               )
              )
            len++;
              /* update the screen, and pdc_lastscr */

              if (len)
              {
            	  trasform_line(line, first, len, src + first);
                  memcpy(dest + first, src + first, len * sizeof(uint16_t));
                  first += len;
                  ret = true;
              }

              /* skip over runs of unchanged cells */

              while (first <= last && src[first] == dest[first])
                  first++;
    }
    return ret;
}

#if 0
	size_t begin_x=0, end_x=0;
	uint16_t* p = (uint16_t*)&screendata[(line * TXT_SCREEN_W)];// * 2];
	uint16_t* pp = (uint16_t*)&prev_screendata[(line * TXT_SCREEN_W)];// * 2];
	while(p[begin_x] == pp[begin_x]) {
		if(++begin_x >=TXT_SCREEN_W) return false;
	}
	end_x = TXT_SCREEN_W -1;
	while(begin_x <  end_x && p[end_x] == pp[end_x]) end_x--;
	assert(begin_x < end_x); // how we get here?
	buffer_clear(&screen_state.buffer);
	ANSI_POS(begin_x,line);
	int last_attrib;
	init_attrib(last_attrib=p[begin_x] >> 8,NULL); // do attribs
	while(begin_x <= end_x)	{
		init_attrib(p[begin_x] >> 8,&last_attrib); // do attribs
		uint8_t c = p[begin_x] & 0xFF;
		vga_to_utf8(c);
		pp[begin_x] = p[begin_x];
		++begin_x;
	}
	return true;
#endif



void USARTx_IRQHandler(void)
{
	  uint32_t isrflags   = READ_REG(USART1->ISR);
	  uint32_t cr1its     = READ_REG(USART1->CR1);
	  uint8_t c;
	  /* If no error occurs */
  uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));

	/* UART in mode Receiver ---------------------------------------------------*/
	if(errorflags == RESET) {
		if((((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))){
			c = USART1->RDR;
			circle_putc(&screen_state.rx_buffer,c);
			printf("CHAR: '%c'(%2.2X)\n",c,c);
			return;
		} else if((((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))){
			if(circle_getc(&screen_state.tx_buffer,&c)){
				USART1->TDR = c;
			} else {
			      /* Disable the UART Transmit Data Register Empty Interrupt */
			      CLEAR_BIT(txt_usart.Instance->CR1, USART_CR1_TXEIE);

			      /* Enable the UART Transmit Complete Interrupt */
			      SET_BIT(txt_usart.Instance->CR1, USART_CR1_TCIE);
			}
			return;
		} else if((((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))){
		      CLEAR_BIT(txt_usart.Instance->CR1, USART_CR1_TXEIE);
#if 0
		      // restart refresh?
#endif
		   return;
		}
	}
	HAL_UART_IRQHandler(&txt_usart);
}
void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(txt_usart.hdmatx);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle == &txt_usart){
		if(screen_state.update)
			screen_state.update();
		else
			screen_state.updating = 0;
	}
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
	txt_usart.Init.BaudRate = 115200;
	//txt_usart.Init.BaudRate =57600;
	//txt_usart.Init.BaudRate =9600;
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

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
   SET_BIT(txt_usart.Instance->CR3, USART_CR3_EIE);

   /* Enable the UART Parity Error and Data Register not empty Interrupts */
   SET_BIT(txt_usart.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
   // enaable reciving stuff
   // screendata = malloc(TXT_SCREEN_W * TXT_SCREEN_H * 2);
   for(size_t i=0; i < (TXT_SCREEN_W * TXT_SCREEN_H * 2); i+=2){
	   prev_screendata[i] = prev_screendata[i+1] = 0;
	   screendata[i]= ' ';
	   screendata[i+1] = 0x7;
   }
    //memset(screendata, 0, TXT_SCREEN_W * TXT_SCREEN_H * 2);
   // memset(prev_screendata, 0, TXT_SCREEN_W * TXT_SCREEN_H * 2);
   circle_init(&screen_state.tx_buffer, dma_tx_buffer,sizeof(dma_tx_buffer));
    circle_init(&screen_state.rx_buffer, dma_rx_buffer,sizeof(dma_rx_buffer));
    uart_puts("Usart works!\n");
   // test_uart("Usart works!\n");


    uart_puts("\033c");	  // reset term
    uart_puts("\033[2J\033[;H");   // clear the screen

    // enable mouse
   // uart_puts("\033[?1000h"); // enable mouse button press?
    uart_puts("\033+C"); // enable mouse tracking?

    uart_puts("\033[?1002h"); // enable mouse movement tracking
    // TXT_UpdateScreen();
    return 1;
}

void TXT_Shutdown(void)
{
   // free(screendata);
  //  screendata = NULL;
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


void TXT_UpdateScreen(void)
{
#if 0
	screen_wait();
	screen_state.updating = 1;
	last_attrib=-1;
	current_line=0;
	screen_state.update = updating_screen_pump;
	updating_screen_pump();
#endif
	// lets just do a full debug update on the screen
#if 0
	last_attrib=-1;
	for(int line =0; line < TXT_SCREEN_H;line++ ){
		circle_clear(&screen_state.tx_buffer);
		trasform_line(line,0,TXT_SCREEN_W, &screendata[(line * TXT_SCREEN_W)*2]);
		uart_write_circle(&screen_state.tx_buffer);

	}
	return;
#endif

	screen_state.update=NULL;
	circle_clear(&screen_state.tx_buffer);
	for(int line =0; line < TXT_SCREEN_H;line++ ){
		if(!do_update(line)) continue;
		screen_state.updating = 1;
		uart_write_circle(&screen_state.tx_buffer);
		screen_state.updating = 0;
		//uart_write(screen_s)
		//trasmit_buffer();
		//screen_wait();
		circle_clear(&screen_state.tx_buffer);
	}
	screen_state.update = NULL;
	screen_state.updating = 0;
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
static int cur_x=0;
static int cur_y=0;
void TXT_GetMousePosition(int *x, int *y)
{
    //SDL_GetMouseState(x, y);
	*x = cur_x;
	*y = cur_y;
#if 0
    *x /= font->w;
    *y /= font->h;
#endif
}
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
static signed int ANSIButtonToTXTButton(uint8_t state, uint8_t x, uint8_t y)
{
	cur_x = x - 32 -1;
	cur_y = y - 32 -1; // - space - 1 based

	uint8_t button = state & 0x3;
	uint8_t shift = state & 0x4;
	uint8_t alt = state & 0x8;
	uint8_t ctrl = state & 0x10;
	uint8_t motion = state & 0x20;
	printf("Mouse click s=%x b=%u x=%u, x=y%u\n", state, button, cur_x, cur_y);
	//if(motion && button)
	switch (button)
	{
		case 0:
			return TXT_MOUSE_LEFT;
		case 1:
			return TXT_MOUSE_RIGHT;
		case 2:
			return TXT_MOUSE_MIDDLE;
		default:
			 if (motion && MouseHasMoved()) return 0;
			// 3 is released
			break;
	}
    return -1;
}

signed int TXT_GetChar(void)
{
	uint8_t c=0;
	uint8_t asci_buffer[8];
	if(circle_getc(&screen_state.rx_buffer,&c)){
		if(c == 27) { // escape code
			HAL_Delay(10);// wait a millsecond
			if(!circle_getc(&screen_state.rx_buffer,&c)){
				return KEY_ESCAPE;
			}
			if(c == '[') {
				// csi
				while(!circle_getc(&screen_state.rx_buffer,&c));
				switch(c) {
				case 'A': return KEY_UPARROW;
				case 'B': return KEY_DOWNARROW;
				case 'C': return KEY_RIGHTARROW;
				case 'D': return KEY_LEFTARROW;
				case 'M': // mouse update
					// we are waiting on 3 bytes after this
					while(!circle_getc(&screen_state.rx_buffer,asci_buffer + 0));
					while(!circle_getc(&screen_state.rx_buffer,asci_buffer + 1));
					while(!circle_getc(&screen_state.rx_buffer,asci_buffer + 2));
					return ANSIButtonToTXTButton(asci_buffer[0],asci_buffer[1],asci_buffer[2]); // update the mouse
				default:
					debug_char("Unkonwn escape ", c);
					return -1;
				}
			}
			debug_char("Unkonwn csiescape ", c);
			return -1;
		}
	}
	return c;
}

#if 0
    Cursor up            ku         stuff \033[A
                                    stuff \033OA      (A)
    Cursor down          kd         stuff \033[B
                                    stuff \033OB      (A)
    Cursor right         kr         stuff \033[C
                                    stuff \033OC      (A)
    Cursor left          kl         stuff \033[D
                                    stuff \033OD      (A)
#endif
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
    	while(circle_empty(&screen_state.rx_buffer)){
    		TXT_UpdateScreen();
    	}
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

