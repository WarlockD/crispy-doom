// ID_VL.H

// wolf compatability

void Quit (const char *error,...);

//===========================================================================

#define CHARWIDTH		2
#define TILEWIDTH		4

//===========================================================================

//extern Wolf_Surface *screen, *screenBuffer, *curSurface;
extern Wolf_Surface* wolf_screen;

extern  boolean  fullscreen, usedoublebuffering;
extern  unsigned screenWidth, screenHeight, screenBits, screenPitch, bufferPitch, curPitch;
extern  unsigned scaleFactor;

extern	boolean  screenfaded;
extern	unsigned bordercolor;

extern Wolf_Color gamepal[256];

//===========================================================================

//
// VGA hardware routines
//

#define VL_WaitVBL(a) DelayMS((a)*8)

void VL_SetVGAPlaneMode (void);
void VL_SetTextMode (void);
void VL_Shutdown (void);

void VL_ConvertPalette(byte *srcpal, Wolf_Color *destpal, int numColors);
void VL_FillPalette (int red, int green, int blue);
void VL_SetColor    (int color, int red, int green, int blue);
void VL_GetColor    (int color, int *red, int *green, int *blue);
void VL_SetPalette  (Wolf_Color *palette, bool forceupdate);
void VL_GetPalette  (Wolf_Color *palette);
void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps);
void VL_FadeIn      (int start, int end, Wolf_Color *palette, int steps);

void VL_RefreshScreen();

#if 0
byte *VL_LockSurface(Wolf_Surface *surface);
void VL_UnlockSurface(Wolf_Surface *surface);

#define LOCK() (VL_LockSurface(wolf_screen))
#define UNLOCK() (VL_UnlockSurface(wolf_screen))
#endif

byte VL_GetPixel        (int x, int y);
void VL_Plot            (int x, int y, int color);
void VL_Hlin            (unsigned x, unsigned y, unsigned width, int color);
void VL_Vlin            (int x, int y, int height, int color);
void VL_BarScaledCoord  (int scx, int scy, int scwidth, int scheight, int color);
void inline VL_Bar      (int x, int y, int width, int height, int color)
{
    VL_BarScaledCoord(scaleFactor*x, scaleFactor*y,
        scaleFactor*width, scaleFactor*height, color);
}
void VL_ClearScreen(int color);

void VL_MungePic                (byte *source, unsigned width, unsigned height);
void VL_DrawPicBare             (int x, int y, byte *pic, int width, int height);
void VL_MemToLatch              (byte *source, int width, int height,
                                    Wolf_Surface *destSurface, int x, int y);
void VL_ScreenToScreen          (Wolf_Surface *dest,const Wolf_Surface *source);
void VL_MemToScreenScaledCoord  (byte *source, int width, int height, int scx, int scy);
void VL_MemToScreenScaledCoord  (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                    int destx, int desty, int width, int height);

void inline VL_MemToScreen (byte *source, int width, int height, int x, int y)
{
    VL_MemToScreenScaledCoord(source, width, height,
        scaleFactor*x, scaleFactor*y);
}

void VL_MaskedToScreen (byte *source, int width, int height, int x, int y);

void VL_LatchToScreenScaledCoord (Wolf_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest);

void inline VL_LatchToScreen (Wolf_Surface *source, int xsrc, int ysrc,
    int width, int height, int xdest, int ydest)
{
    VL_LatchToScreenScaledCoord(source,xsrc,ysrc,width,height,
        scaleFactor*xdest,scaleFactor*ydest);
}
void inline VL_LatchToScreenScaledCoord (Wolf_Surface *source, int scx, int scy)
{
    VL_LatchToScreenScaledCoord(source,0,0,source->w,source->h,scx,scy);
}
void inline VL_LatchToScreen (Wolf_Surface *source, int x, int y)
{
    VL_LatchToScreenScaledCoord(source,0,0,source->w,source->h,
        scaleFactor*x,scaleFactor*y);
}
