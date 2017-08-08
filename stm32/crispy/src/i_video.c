#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>


#include "crispy.h"

#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_scale.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_diskicon.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"


#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"


#define SCALE_2X
#define GFX_RGB565(r, g, b)			((((r & 0xF8) >> 3) << 11) | (((g & 0xFC) >> 2) << 5) | ((b & 0xF8) >> 3))

#define GFX_RGB565_R(color)			((0xF800 & color) >> 11)
#define GFX_RGB565_G(color)			((0x07E0 & color) >> 5)
#define GFX_RGB565_B(color)			(0x001F & color)

#define GFX_ARGB8888(r, g, b, a)	(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))

#define GFX_ARGB8888_R(color)		((color & 0x00FF0000) >> 16)
#define GFX_ARGB8888_G(color)		((color & 0x0000FF00) >> 8)
#define GFX_ARGB8888_B(color)		((color & 0x000000FF))
#define GFX_ARGB8888_A(color)		((color & 0xFF000000) >> 24)

#define GFX_TRANSPARENT				0x00
#define GFX_OPAQUE					0xFF
// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;
#ifdef SCALE_2X
byte* I_VideoBufferX2 = NULL;
#endif

typedef struct
{
	byte r;
	byte g;
	byte b;
} col_t;

// Palette converted to RGB565

//static uint16_t rgb565_palette[256];
static uint32_t rgb888_palette[256];
static boolean run;
//extern uint32_t* layer0_address;
extern DMA2D_HandleTypeDef hdma2d_eval;
extern LTDC_HandleTypeDef  hltdc_eval;
//extern DSI_HandleTypeDef hdsi_eval;

#define DMA2D_WORKING               ((DMA2D->CR & DMA2D_CR_START))
#define DMA2D_WAIT                  do { while (DMA2D_WORKING); DMA2D->IFCR = DMA2D_IFSR_CTCIF;} while (0);
#define DMA2D_CLUT_WORKING          ((DMA2D->FGPFCCR & DMA2D_FGPFCCR_START))
#define DMA2D_CLUT_WAIT              do { while (DMA2D_CLUT_WORKING); DMA2D->IFCR = DMA2D_IFSR_CCTCIF;} while (0);

boolean initialized = false;

volatile boolean pallete_load_complete = true;
volatile boolean frame_finished = true;
void HAL_DMA2D_CLUTLoadingCpltCallback(DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);
  pallete_load_complete =true;

}
void HAL_FrameFinished(DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);
  frame_finished =true;

}
void HAL_FrameError(DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);
  while(1) {
	  BSP_LED_Toggle(LED_GREEN);
	  HAL_Delay(1);
  }

}

void SetupDMA() {
	DMA2D_WAIT;
	hdma2d_eval.Instance = DMA2D;
	//  hdma2d.Init.Mode = DMA2D_M2M;
	hdma2d_eval.Init.Mode = DMA2D_M2M_PFC;
	hdma2d_eval.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
#ifdef SCALE_2X
	hdma2d_eval.Init.OutputOffset = 800-(SCREENWIDTH*2);
#else
	hdma2d_eval.Init.OutputOffset = 800-SCREENWIDTH;
#endif
	hdma2d_eval.XferCpltCallback = HAL_FrameFinished;
	hdma2d_eval.XferErrorCallback = 	HAL_FrameError;
	hdma2d_eval.LayerCfg[1].InputOffset = 0;
	 // hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;

	hdma2d_eval.LayerCfg[1].InputColorMode =DMA2D_INPUT_L8;
	hdma2d_eval.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d_eval.LayerCfg[1].InputAlpha = 0;
	if (HAL_DMA2D_Init(&hdma2d_eval) != HAL_OK)  Error_Handler();
	if (HAL_DMA2D_ConfigLayer(&hdma2d_eval, 1) != HAL_OK)  Error_Handler();

}


void I_InitGraphics() {

	I_VideoBuffer = (byte*)Z_Malloc (SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);
#ifdef SCALE_2X
	I_VideoBufferX2 = (byte*)Z_Malloc (SCREENWIDTH * SCREENHEIGHT*4, PU_STATIC, NULL);
#endif
	screenvisible = true;
	SetupDMA();
}
void I_ShutdownGraphics(void) {}

//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}
void I_GetEvent(void)
{


}
//
// I_StartTic
//
void I_StartTic (void)
{
    if (!initialized)
    {
        return;
    }

    I_GetEvent();
#if 0
    if (usemouse && !nomouse)
    {
        I_ReadMouse();
    }
#endif
    I_UpdateJoystick();
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

void TM_INT_DMA2DGRAPHIC_InitAndTransfer(void) {
	/* Wait until transfer is done first from other calls */
	DMA2D_WAIT;

	/* DeInit DMA2D */
	RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2DRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2DRST;

	/* Initialize DMA2D */
	//DMA2D_Init(&GRAPHIC_DMA2D_InitStruct);

	/* Start transfer */
	DMA2D->CR |= (uint32_t)DMA2D_CR_START;

	/* Wait till transfer ends */
	DMA2D_WAIT;
}

void UpdateNoScale() {
	assert(HAL_DMA2D_PollForTransfer(&hdma2d_eval,50) == HAL_OK);
	// MODIFY_REG(hdma2d->Instance->OOR, DMA2D_OOR_LO, 800-SCREENWIDTH);
	HAL_DMA2D_Start(&hdma2d_eval, (uint32_t)I_VideoBuffer, (hltdc_eval.LayerCfg[0].FBStartAdress), SCREENWIDTH,  SCREENHEIGHT);
}
void DoubleScanLIne(uint8_t* src, uint8_t* dst, uint32_t original_size){
	for (int x = 0; x < original_size; x++){
		*dst++ = *src; *dst++ = *src++;
	}
}
void DoubleScreen(uint8_t* src, uint8_t* dst, uint32_t width, uint32_t height){
	uint32_t nwidth = width*2;
	uint32_t nheight = height*2;
	for (uint32_t y = 0; y < height; y++){
		uint8_t* dst_start = dst;
		for (int x = 0; x < width; x++){
			dst[0] = dst[1] = *src;
			//dst[nwidth] = dst[nwidth+1] = *src;
			src++;
			dst+=2;
			//*dst++ = *src; *dst++ = *src++;
		}
		memcpy(dst, dst_start, nwidth);
		dst+= nwidth;
		//memcpy(dst, start_src, nwidth);
	}
}
void Update2XScale() {
	assert(HAL_DMA2D_PollForTransfer(&hdma2d_eval,50) == HAL_OK);
	uint32_t* display_start = (uint32_t*)(hltdc_eval.LayerCfg[0].FBStartAdress);// + 4 * y * tft_width);
	//   (uint8_t*)(display_start + (tft_width*tft_height)); // I_VideoBufferX2;
	DoubleScreen(I_VideoBuffer, I_VideoBufferX2, SCREENWIDTH, SCREENHEIGHT);
//	MODIFY_REG(DMA2D->OOR, DMA2D_OOR_LO, 800-(SCREENWIDTH*2));
	HAL_DMA2D_Start(&hdma2d_eval, (uint32_t)I_VideoBufferX2, (uint32_t)display_start, SCREENWIDTH*2,  SCREENHEIGHT*2);
}

void I_FinishUpdate (void)
{

	DMA2D_WAIT;
	//assert(HAL_DMA2D_PollForTransfer(&hdma2d_eval,50) == HAL_OK);
#ifdef SCALE_2X
	Update2XScale();
#else
	UpdateNoScale();
#endif
	DMA2D_WAIT;
}
//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
	DMA2D_CLUT_WAIT;
	DMA2D_WAIT;

	//assert(HAL_DMA2D_PollForTransfer(&hdma2d_eval,50) == HAL_OK);
	//while(!pallete_load_complete);
//	pallete_load_complete = false;
	int i;
	col_t* c;
	boolean palette_changed = false;
	for (i = 0; i < 256; i++)
	{
		c = (col_t*)palette;

		uint32_t nc = GFX_ARGB8888(c->r, c->g,c->b,GFX_OPAQUE);
		if(nc != rgb888_palette[i]){
			rgb888_palette[i] = nc;
			palette_changed = true;
		}
		//rgb565_palette[i] = GFX_RGB565(gammatable[usegamma][c->r], gammatable[usegamma][c->g],gammatable[usegamma][c->b]);
		palette += 3;
	}
	if(palette_changed){
		DMA2D_CLUTCfgTypeDef lut;
		lut.CLUTColorMode = DMA2D_CCM_ARGB8888;
		lut.Size = 0xFF;
		lut.pCLUT = rgb888_palette;

		HAL_DMA2D_ConfigCLUT(&hdma2d_eval, lut, 1);
		HAL_DMA2D_EnableCLUT(&hdma2d_eval, 1);
		DMA2D_CLUT_WAIT;
	//SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);
		//assert(HAL_DMA2D_PollForTransfer(&hdma2d_eval,50) == HAL_OK);
	}

	//while(HAL_DMA2D_PollForTransfer(&hdma2d_eval))
}
// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex (int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
    //	color.r = GFX_RGB565_R(rgb565_palette[i]);
    //	color.g = GFX_RGB565_G(rgb565_palette[i]);
    //	color.b = GFX_RGB565_B(rgb565_palette[i]);
    	color.r = GFX_ARGB8888_R(rgb888_palette[i]);
    	color.g = GFX_ARGB8888_G(rgb888_palette[i]);
    	color.b = GFX_ARGB8888_B(rgb888_palette[i]);

        diff = (r - color.r) * (r - color.r)
             + (g - color.g) * (g - color.g)
             + (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}
void I_BeginRead (void)
{
}

void I_EndRead (void)
{
}

void I_SetWindowTitle (char *title)
{
}

void I_GraphicsCheckCommandLine (void)
{
}

void I_SetGrabMouseCallback (grabmouse_callback_t func)
{
	(void)func;
}
// Enable the loading disk image displayed when reading from disk.


void I_EnableLoadingDisk (int xoffs, int yoffs)
{
	(void)xoffs; (void)yoffs;
}

void I_BindVideoVariables (void)
{
}

void I_DisplayFPSDots (boolean dots_on)
{
	(void)dots_on;
}

void I_CheckIsScreensaver (void)
{
}


