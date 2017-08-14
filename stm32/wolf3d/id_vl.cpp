// ID_VL.C

#include <string.h>
#include "wl_def.h"
#include <stdarg.h>
#include "z_zone.h"

#pragma hdrstop

// Uncomment the following line, if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//#define IGNORE_BAD_DEST
#ifdef USE_STM32746G_DISCO
#include "stm32746g_discovery_lcd.h"




#endif

#ifdef IGNORE_BAD_DEST
#undef assert
#define assert(x) if(!(x)) return
#define assert_ret(x) if(!(x)) return 0
#else
#define assert_ret(x) assert(x)
#endif

boolean fullscreen = true;
#if defined(_arch_dreamcast)
boolean usedoublebuffering = false;
unsigned screenWidth = 320;
unsigned screenHeight = 200;
unsigned screenBits = 8;
#elif defined(GP2X)
boolean usedoublebuffering = true;
unsigned screenWidth = 320;
unsigned screenHeight = 240;
#if defined(GP2X_940)
unsigned screenBits = 8;
#else
unsigned screenBits = 16;
#endif
#else

unsigned screenWidth = 640;
unsigned screenHeight = 400;
unsigned screenBits = -1;      // use "best" color depth according to libSDL
#endif

static volatile Wolf_Color* lcd_screen= (Wolf_Color*)LCD_FB_START_ADDRESS;
Wolf_Surface* wolf_screen;
Wolf_Color palette1[256], palette2[256];
Wolf_Color curpal[256];

#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

Wolf_Color gamepal[]={
#ifdef SPEAR
    #include "sodpal.inc"
#else
    #include "wolfpal.inc"
#endif
};

// for I zone
extern "C" void I_Error(const char* msg, ...){
	va_list va;
	va_start(va,msg);
	vfprintf(stderr,msg,va);
	va_end(va);
	assert(0);
	while(1) {};
}

extern "C"  void* I_ZoneBase (int *size) {
	lcd_screen = (Wolf_Color*)LCD_FB_START_ADDRESS;
	uint32_t start_address = LCD_FB_START_ADDRESS;
	// get passed atleast layer one
	start_address += 1024*1024;
	*size = 1024*1024*6;
	return (byte*)start_address;
}

void VL_RefreshScreen() {
	for(int y=0;y < wolf_screen->h;y++){
		Wolf_Color* lcd_line = (Wolf_Color*)&lcd_screen[y * wolf_screen->pitch];
		volatile uint8_t* wolf_line = &wolf_screen->pixels[y * wolf_screen->pitch];
		for(int x=0; x< wolf_screen->w;x++) {
			Wolf_Color c = curpal[wolf_line[x]];
			lcd_line[x] = c;
		}
	}
}





CASSERT(lengthof(gamepal) == 256)
/*
=================
=
= VL_ConvertPalette
=
=================
*/
Wolf_Surface* VL_CreateSurface(uint32_t flags, int width, int height){
	size_t size = sizeof(Wolf_Surface);
	uint32_t pitch = (width+3) & ~3; // 32 bit alligned
	size += pitch * height;

	Wolf_Surface* surface = (Wolf_Surface*)Z_Malloc(size,PU_STATIC,NULL);
	memset(surface,0,size);
	surface->pixels = (uint8_t*)(surface+1);
	surface->w = width;
	surface->h = height;
	surface->pitch = pitch;
	surface->flags = 0;
	return surface;
}
void VL_DeleteSurface(Wolf_Surface* surface){
	Z_Free(surface);
}

void VL_ConvertPalette(byte *srcpal, Wolf_Color *destpal, int numColors)
{
    for(int i=0; i<numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int red, int green, int blue)
{
    int i;
    Wolf_Color pal[256];

    for(i=0; i<256; i++)
    {
        pal[i].r = red;
        pal[i].g = green;
        pal[i].b = blue;
    }

    VL_SetPalette(pal, true);
}
void VL_SetColor	(int color, int red, int green, int blue)
{
    Wolf_Color col;
    col.r = (uint8_t)red;
    col.g = (uint8_t)green;
    col.b = (uint8_t)blue;
    col.a = 0xFF;
    curpal[color] = col;
    VL_RefreshScreen();
#ifdef USE_SDL
    if(screenBits == 8)
        SDL_SetPalette(screen, SDL_PHYSPAL, &col, color, 1);
    else
    {
        SDL_SetPalette(curSurface, SDL_LOGPAL, &col, color, 1);
        SDL_BlitSurface(curSurface, NULL, screen, NULL);
        SDL_Flip(screen);
    }
#endif
}
void VL_GetPalette (Wolf_Color *palette)
{
    memcpy(palette, curpal, sizeof(Wolf_Color) * 256);
}

#ifdef USE_STM32746G_DISCO
#include "stm32f7xx_hal.h"

#define XSIZE_PHYS 480
#define YSIZE_PHYS 272

/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index
  * @retval Status ( 0 : 0k , 1: error)
  */
static void DMA2D_CopyBuffer(int format, void * pSrc, void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLineSrc, uint32_t OffLineDst)
{
	uint32_t PixelFormat;

 // PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
  PixelFormat = format; //  LTDC_PIXEL_FORMAT_ARGB8888;

  DMA2D->CR      = 0x00000000UL | (1 << 9);

  /* Set up pointers */
  DMA2D->FGMAR   = (uint32_t)pSrc;
  DMA2D->OMAR    = (uint32_t)pDst;
  DMA2D->FGOR    = OffLineSrc;
  DMA2D->OOR     = OffLineDst;

  /* Set up pixel format */
  DMA2D->FGPFCCR = PixelFormat;

  /*  Set up size */
  DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;

  DMA2D->CR     |= DMA2D_CR_START;

  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START)
  {
  }
}
static void DMA2D_FillBuffer(int format, void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{

	uint32_t PixelFormat;

	  PixelFormat = format; //  LTDC_PIXEL_FORMAT_ARGB8888;

  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);
  DMA2D->OCOLR   = ColorIndex;

  /* Set up pointers */
  DMA2D->OMAR    = (uint32_t)pDst;

  /* Set up offsets */
  DMA2D->OOR     = OffLine;

  /* Set up pixel format */
  DMA2D->OPFCCR  = PixelFormat;

  /*  Set up size */
  DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;

  DMA2D->CR     |= DMA2D_CR_START;

  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START)
  {
  }
}
#endif
/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void	VL_Shutdown (void)
{
	//VL_SetTextMode ();
}

void  VL_ClearScreen(int color)
{
#ifdef USE_SDL
    SDL_FillRect(curSurface, NULL, color);
#endif
#ifdef USE_STM32746G_DISCO
    BSP_LCD_Clear( (*(uint32_t*)&curpal[color]));
#endif
}
void VL_ScreenToScreen (Wolf_Surface *dest, const Wolf_Surface *source)
{
	int w = source->w > dest->w ? dest->w : source->w;
	int h = source->h > dest->h ? dest->h : source->h;

	for(size_t y=0;y < h;y++){
		const uint8_t* src_line = &source->pixels[y * source->pitch];
		uint8_t* dest_line = &dest->pixels[y * dest->pitch];
		memcpy(dest_line,src_line,w);
	}
}

void VL_SetPalette (Wolf_Color *palette, bool forceupdate)
{
    memcpy(curpal, palette, sizeof(Wolf_Color) * 256);
    VL_RefreshScreen();
}

//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
	int		    i,j,orig,delta;
	Wolf_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(Wolf_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		origptr = &palette1[start];
		newptr = &palette2[start];
		for (j=start;j<=end;j++)
		{
			orig = origptr->r;
			delta = red-orig;
			newptr->r = orig + delta * i / steps;
			orig = origptr->g;
			delta = green-orig;
			newptr->g = orig + delta * i / steps;
			orig = origptr->b;
			delta = blue-orig;
			newptr->b = orig + delta * i / steps;
			origptr++;
			newptr++;
		}

		if(!usedoublebuffering || screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette (palette2, true);
	}

//
// final color
//
	VL_FillPalette (red,green,blue);

	screenfaded = true;
}

//===========================================================================
void	VL_SetVGAPlaneMode (void)
{
	wolf_screen = VL_CreateSurface(screenWidth,screenHeight);
#if 0
    curSurface = screenBuffer;
    curPitch = bufferPitch;

    scaleFactor = screenWidth/320;
    if(screenHeight/200 < scaleFactor) scaleFactor = screenHeight/200;

    pixelangle = (short *) malloc(screenWidth * sizeof(short));
    CHECKMALLOCRESULT(pixelangle);
    wallheight = (int *) malloc(screenWidth * sizeof(int));
    CHECKMALLOCRESULT(wallheight);
#endif
}

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor	(int color, int *red, int *green, int *blue)
{
    Wolf_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}
#if 0
byte *VL_LockSurface(Wolf_Surface *surface)
{

    if(++surface->lock == 1) return surface->pixels;
    --surface->lock;
    return NULL;
}

void VL_UnlockSurface(Wolf_Surface *surface)
{
    surface->lock=0;

}
#endif
static inline uint8_t* GetSurfacePtr(Wolf_Surface* surface, int x, int y) {
    assert(x >= 0 && (unsigned) x < surface->w
            && y >= 0 && (unsigned) y < surface->h
            && "Pixel out of bounds!");
	return surface->pixels + y * surface->pitch + x;
}
static inline uint8_t* GetScreenPtr(int x, int y) {
	return GetSurfacePtr(wolf_screen,x,y);
}
/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int x, int y, int color)
{
    *GetScreenPtr(x,y)= (uint8_t)color;
}

/*
=================
=
= VL_GetPixel
=
=================
*/

byte VL_GetPixel (int x, int y)
{
	return  *GetScreenPtr(x,y);
}


/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    assert(x >= 0 && x + width <= screenWidth
            && y >= 0 && y < screenHeight
            && "VL_Hlin: Destination rectangle out of bounds!");
    memset(GetScreenPtr(x,y), color, width);
}


/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin (int x, int y, int height, int color)
{
	assert(x >= 0 && (unsigned) x < screenWidth
			&& y >= 0 && (unsigned) y + height <= screenHeight
			&& "VL_Vlin: Destination rectangle out of bounds!");
	 byte *dest = GetScreenPtr(x,y);

	while (height--)
	{
		*dest = color;
		dest += wolf_screen->pitch;
	}
}


/*
=================
=
= VL_Bar
=
=================
*/

void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
	assert(scx >= 0 && (unsigned) scx + scwidth <= screenWidth
			&& scy >= 0 && (unsigned) scy + scheight <= screenHeight
			&& "VL_BarScaledCoord: Destination rectangle out of bounds!");
	 byte *dest = GetScreenPtr(scx,scy);  //curSurface->pixels + scy * wolf_screen->pitch + scx;

	while (scheight--)
	{
		memset(dest, color, scwidth);
		dest += wolf_screen->pitch;
	}
}

/*
============================================================================

							MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch(byte *source, int width, int height,
    Wolf_Surface *destSurface, int x, int y)
{
    assert(x >= 0 && (unsigned) x + width <= screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_MemToLatch: Destination rectangle out of bounds!");

    int pitch = destSurface->pitch;
    byte *dest = GetSurfacePtr(destSurface,x,y);
    for(int ysrc = 0; ysrc < height; ysrc++)
    {
        for(int xsrc = 0; xsrc < width; xsrc++)
        {
            dest[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
                + (xsrc & 3) * (width >> 2) * height];
        }
    }
}

//===========================================================================


/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a block of data to the screen with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    byte *vbuf = GetScreenPtr(0,0);
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[(j*(width>>2)+(i>>2))+(i&3)*(width>>2)*height];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
}

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a part of a block of data to the screen.
= The block has the size origwidth*origheight.
= The part at (srcx, srcy) has the size width*height
= and will be painted to (destx, desty) with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    byte *vbuf = GetScreenPtr(0,0);
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[((j+srcy)*(origwidth>>2)+((i+srcx)>>2))+((i+srcx)&3)*(origwidth>>2)*origheight];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreenScaledCoord(Wolf_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
	assert(scxdest >= 0 && scxdest + width * scaleFactor <= screenWidth
			&& scydest >= 0 && scydest + height * scaleFactor <= screenHeight
			&& "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

	if(scaleFactor == 1)
    {
        // HACK: If screenBits is not 8 and the screen is faded out, the
        //       result will be black when using SDL_BlitSurface. The reason
        //       is that the logical palette needed for the transformation
        //       to the screen color depth is not equal to the logical
        //       palette of the latch (the latch is not faded). Therefore,
        //       SDL tries to map the colors...
        //       The result: All colors are mapped to black.
        //       So, we do the blit on our own...
            byte *src = (byte *) source->pixels;
            unsigned srcPitch = source->pitch;

            byte *vbuf = GetScreenPtr(0,0);
            for(int j=0,scj=0; j<height; j++, scj++)
            {
                for(int i=0,sci=0; i<width; i++, sci++)
                {
                    byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                    vbuf[(scydest+scj)*wolf_screen->pitch+scxdest+sci] = col;
                }
            }
    }
    else
    {
        byte *src = (byte *) source->pixels;
        unsigned srcPitch = source->pitch;

        byte *vbuf = GetScreenPtr(0,0);
        for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
        {
            for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
            {
                byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                for(unsigned m=0; m<scaleFactor; m++)
                {
                    for(unsigned n=0; n<scaleFactor; n++)
                    {
                        vbuf[(scydest+scj+m)*curPitch+scxdest+sci+n] = col;
                    }
                }
            }
        }
    }
}
#if 0
void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
	int		    i,j,orig,delta;
	SDL_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		origptr = &palette1[start];
		newptr = &palette2[start];
		for (j=start;j<=end;j++)
		{
			orig = origptr->r;
			delta = red-orig;
			newptr->r = orig + delta * i / steps;
			orig = origptr->g;
			delta = green-orig;
			newptr->g = orig + delta * i / steps;
			orig = origptr->b;
			delta = blue-orig;
			newptr->b = orig + delta * i / steps;
			origptr++;
			newptr++;
		}

		if(!usedoublebuffering || screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette (palette2, true);
	}

//
// final color
//
	VL_FillPalette (red,green,blue);

	screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, SDL_Color *palette, int steps)
{
	int i,j,delta;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		for (j=start;j<=end;j++)
		{
			delta = palette[j].r-palette1[j].r;
			palette2[j].r = palette1[j].r + delta * i / steps;
			delta = palette[j].g-palette1[j].g;
			palette2[j].g = palette1[j].g + delta * i / steps;
			delta = palette[j].b-palette1[j].b;
			palette2[j].b = palette1[j].b + delta * i / steps;
		}

		if(!usedoublebuffering || screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette(palette2, true);
	}

//
// final color
//
	VL_SetPalette (palette, true);
	screenfaded = false;
}

/*
=============================================================================

							PIXEL OPS

=============================================================================
*/

byte *VL_LockSurface(SDL_Surface *surface)
{
#ifdef USE_SDL
    if(SDL_MUSTLOCK(surface))
    {
        if(SDL_LockSurface(surface) < 0)
            return NULL;
    }
#endif
    return (byte *) surface->pixels;
}

void VL_UnlockSurface(SDL_Surface *surface)
{
#ifdef USE_SDL
    if(SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
#else
    (void)surface;
#endif
}




//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen (SDL_Surface *source, SDL_Surface *dest)
{
    SDL_BlitSurface(source, NULL, dest, NULL);
}

#endif
