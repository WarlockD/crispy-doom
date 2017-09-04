/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// vid_null.c -- null video driver to aid porting efforts



#include "quakedef.h"
#include "winquake.h"
#include "d_local.h"
#include "resource.h"

#include <cstdint>
#include <atlbase.h>
#include <atlwin.h>

#include "../wtl/Include/atlapp.h"
#include "../wtl/Include/atlscrl.h"

#include "../wtl/Include/atlframe.h"


#define MODE_WINDOWED			0
#define MODE_SETTABLE_WINDOW	2
#define NO_MODE					(MODE_WINDOWED - 1)
#define MODE_FULLSCREEN_DEFAULT	(MODE_WINDOWED + 3)

short *d_pzbuffer;
int			window_center_x, window_center_y, window_x, window_y, window_width, window_height;
RECT		window_rect;

// Note that 0 is MODE_WINDOWED
cvar_t		vid_mode = { "vid_mode","0", false };
// Note that 0 is MODE_WINDOWED
cvar_t		_vid_default_mode = { "_vid_default_mode","0", true };
// Note that 3 is MODE_FULLSCREEN_DEFAULT
cvar_t		_vid_default_mode_win = { "_vid_default_mode_win","3", true };
cvar_t		vid_wait = { "vid_wait","0" };
cvar_t		vid_nopageflip = { "vid_nopageflip","0", true };
cvar_t		_vid_wait_override = { "_vid_wait_override", "0", true };
cvar_t		vid_config_x = { "vid_config_x","800", true };
cvar_t		vid_config_y = { "vid_config_y","600", true };
cvar_t		vid_stretch_by_2 = { "vid_stretch_by_2","1", true };
cvar_t		_windowed_mouse = { "_windowed_mouse","0", true };
cvar_t		vid_fullscreen_mode = { "vid_fullscreen_mode","3", true };
cvar_t		vid_windowed_mode = { "vid_windowed_mode","0", true };
cvar_t		block_switch = { "block_switch","0", true };
cvar_t		vid_window_x = { "vid_window_x", "0", true };
cvar_t		vid_window_y = { "vid_window_y", "0", true };



viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	200



unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

void CBitmapWindow::VID_Update(vrect_t *rects)
{
	// just update the vid since its the same as the pixels?
	//Update(vid.buffer);
	//return;
	CDCHandle hdc = this->GetDC();
	auto& buffer = *backbuffer();
	HBITMAP hBmpOld = hdc.SelectBitmap(buffer);

	while (rects)
	{
		auto ycount = rects->height;
		auto offset = rects->y * vid.rowbytes + rects->x;
		
		for (int y = 0; y < rects->height; y++) {
			const uint8_t* line = vid.buffer + ((y + rects->y) * vid.rowbytes) + rects->x;

			for (int x = 0; x < rects->width; x++) {
				hdc.SetPixel(x, y, ref_pal[*line++]);
			}

		}
		rects = rects->pnext;
	}
	hdc.SelectBitmap(hBmpOld);
#if 0
	while (rects)
	{
		buffer.cr
			// have to worry about soo many buffers sigh
			hdc.drawe

			if (vid_stretched)
			{
				MGL_stretchBltCoord(windc, dibdc,
					rects->x, rects->y,
					rects->x + rects->width, rects->y + rects->height,
					rects->x << 1, rects->y << 1,
					(rects->x + rects->width) << 1,
					(rects->y + rects->height) << 1);
			}
			else
			{
				MGL_bitBltCoord(windc, dibdc,
					rects->x, rects->y,
					rects->x + rects->width, rects->y + rects->height,
					rects->x, rects->y, MGL_REPLACE_MODE);
			}

		rects = rects->pnext;
	}
#endif
}


	void VID_HandlePause(qboolean v) {
		(void)v;
	}
	int VID_ForceUnlockedAndReturnState() {
		return 0;
	}
	void 	VID_ForceLockState(int state) {
		(void)state;
	}
	void VID_UnlockBuffer(void) {

	}
	void VID_LockBuffer(void) {

	}
	void	VID_SetPalette(unsigned char *palette)
	{
		wndMain.SetPalette(palette);
	}

	void	VID_ShiftPalette(unsigned char *palette)
	{
		wndMain.SetPalette(palette);
	}

	void	VID_Init(unsigned char *palette)
	{

		wndMain.createbackbuffers(BASEWIDTH, BASEHEIGHT, palette);
		wndMain.VID_Init(palette);
#if 0
		vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
		vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
		vid.aspect = 1.0;
		vid.numpages = 1;
		vid.colormap = host_colormap;
		vid.fullbright = 256 - LittleLong(*((int *)vid.colormap + 2048));
		vid.buffer = vid.conbuffer = vid_buffer;
		vid.rowbytes = vid.conrowbytes = BASEWIDTH;

		d_pzbuffer = zbuffer;
		D_InitCaches(surfcache, sizeof(surfcache));
#endif
		d_pzbuffer = wndMain.zbuffer.data();
		int		i, bestmatch, bestmatchmetric, t, dr, dg, db;
		int		basenummodes;
		byte	*ptmp;

		Cvar_RegisterVariable(&vid_mode);
		Cvar_RegisterVariable(&vid_wait);
		Cvar_RegisterVariable(&vid_nopageflip);
		Cvar_RegisterVariable(&_vid_wait_override);
		Cvar_RegisterVariable(&_vid_default_mode);
		Cvar_RegisterVariable(&_vid_default_mode_win);
		Cvar_RegisterVariable(&vid_config_x);
		Cvar_RegisterVariable(&vid_config_y);
		Cvar_RegisterVariable(&vid_stretch_by_2);
		Cvar_RegisterVariable(&_windowed_mouse);
		Cvar_RegisterVariable(&vid_fullscreen_mode);
		Cvar_RegisterVariable(&vid_windowed_mode);
		Cvar_RegisterVariable(&block_switch);
		Cvar_RegisterVariable(&vid_window_x);
		Cvar_RegisterVariable(&vid_window_y);
	}

	void	VID_Shutdown(void)
	{
	}

	void	VID_Update(vrect_t *rects)
	{
		wndMain.VID_Update(rects);
	}

	/*
	================
	D_BeginDirectRect
	================
	*/
	void D_BeginDirectRect(int x, int y, byte *pbitmap, int width, int height)
	{
	}


	/*
	================
	D_EndDirectRect
	================
	*/
	void D_EndDirectRect(int x, int y, int width, int height)
	{
	}


