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
// winquake.h: Win32-specific Quake header file

#pragma warning( disable : 4229 )  // mgraph gets this

#ifndef _WINQUAKE_H_
#define _WINQUAKE_H_

#include <windows.h>

#define WM_MOUSEWHEEL                   0x020A

#ifndef SERVERONLY
#include <ddraw.h>
#include <dsound.h>
#ifndef GLQUAKE
// to do ugh

#endif
#endif

EXTERN 	HINSTANCE	global_hInstance;
EXTERN 	int			global_nCmdShow;

#ifndef SERVERONLY

EXTERN  LPDIRECTDRAW		lpDD;
EXTERN  qboolean			DDActive;
EXTERN  LPDIRECTDRAWSURFACE	lpPrimary;
EXTERN  LPDIRECTDRAWSURFACE	lpFrontBuffer;
EXTERN  LPDIRECTDRAWSURFACE	lpBackBuffer;
EXTERN  LPDIRECTDRAWPALETTE	lpDDPal;
EXTERN  LPDIRECTSOUND pDS;
EXTERN  LPDIRECTSOUNDBUFFER pDSBuf;

EXTERN  DWORD gSndBufSize;
//#define SNDBUFSIZE 65536

EXTERN_CPP void	VID_LockBuffer (void);
EXTERN_CPP void	VID_UnlockBuffer (void);

#endif


EXTERN  HWND			mainwindow;
EXTERN  qboolean		ActiveApp, Minimized;

EXTERN  qboolean	WinNT;

EXTERN_CPP int VID_ForceUnlockedAndReturnState (void);
EXTERN_CPP void VID_ForceLockState (int lk);

EXTERN_CPP void IN_ShowMouse (void);
EXTERN_CPP void IN_DeactivateMouse (void);
EXTERN_CPP void IN_HideMouse (void);
EXTERN_CPP void IN_ActivateMouse (void);
EXTERN_CPP void IN_RestoreOriginalMouseState (void);
EXTERN_CPP void IN_SetQuakeMouseState (void);
EXTERN_CPP void IN_MouseEvent (int mstate);

EXTERN  qboolean	winsock_lib_initialized;

EXTERN  cvar_t		_windowed_mouse;

EXTERN  int		window_center_x, window_center_y;
EXTERN  RECT		window_rect;

EXTERN  qboolean	mouseinitialized;
EXTERN  HWND		hwnd_dialog;

EXTERN  HANDLE	hinput, houtput;

#ifdef __cplusplus


#include <cstdint>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>

#include "../wtl/Include/atlapp.h"
#include "../wtl/Include/atlscrl.h"

#include "../wtl/Include/atlframe.h"

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	WS_EX_APPWINDOW> CMyWindowTraits;



//class CBitmapWindow : public CWindowImpl<CBitmapWindow, CWindow, CMyWindowTraits>
class CBitmapWindow : public CDoubleBufferWindowImpl<CBitmapWindow, CWindow, CMyWindowTraits>
{
public:
	struct CUSTOM_BITMAPINFO
	{
		// bitmap information
		BITMAPINFOHEADER info;
		/// A 256 colors palette
		RGBQUAD bmiColors[256];
	};
	CUSTOM_BITMAPINFO m_bmp_info;

	std::vector<uint8_t> vid_buffer;
	std::vector<uint8_t> back_buffer;
	std::vector<short> zbuffer;
	std::vector<uint8_t> surfcache;
	COLORREF ref_pal[256];
	CBitmap m_backbuffers[2];
	bool buffers_setup;
	int m_current;
	SIZE m_size;
	DWORD m_stride;

	CBitmapWindow() : buffers_setup(false) {}

	CBitmap* backbuffer() { return m_backbuffers + m_current; }
	void flipbuffer() { m_current = (m_current + 1 & 1); }
	DWORD width() const { return m_size.cx; }
	DWORD height() const { return m_size.cy; }
	DWORD stride() const { return m_stride; }
	void SetPalette(const uint8_t* palette) {
		ATLASSERT(::IsWindow(m_hWnd));
		if (buffers_setup && palette)
		{
			
			for (UINT iColor = 0; iColor < 256; iColor++) {
				uint8_t r = *palette++;
				uint8_t g = *palette++;
				uint8_t b = *palette++;
				ref_pal[iColor] = (r << 16) | (g << 8) | (b);
				m_bmp_info.bmiColors[iColor].rgbRed = r;
				m_bmp_info.bmiColors[iColor].rgbGreen = g;
				m_bmp_info.bmiColors[iColor].rgbBlue = b;
			}
			CDCHandle hdc = this->GetDC();
			
			for (size_t i = 0; i < 2; i++) {
				HBITMAP hBmpOld = hdc.SelectBitmap(m_backbuffers[i]);
				SetDIBColorTable(hdc, 0,256, m_bmp_info.bmiColors);
				hdc.SelectBitmap(hBmpOld);
			}
		}
	}
	void createbackbuffers(size_t width, size_t height, const uint8_t* palette) {
		const size_t bitcount = 8;
		m_size.cx = width;
		m_size.cy = height;
		m_stride = ((((width *bitcount) + 31) &~31) >> 3);
		ZeroMemory(&m_bmp_info, sizeof(CUSTOM_BITMAPINFO));
		BITMAPINFO* info = (BITMAPINFO*)&m_bmp_info;
		m_bmp_info.info.biSize = sizeof(BITMAPINFOHEADER);
		m_bmp_info.info.biHeight = -((int)height);
		m_bmp_info.info.biWidth = width;
		m_bmp_info.info.biPlanes = 1;
		m_bmp_info.info.biBitCount = 8;
		m_bmp_info.info.biCompression = BI_RGB;
		m_bmp_info.info.biSizeImage = 0;
		m_bmp_info.info.biXPelsPerMeter = 0;
		m_bmp_info.info.biYPelsPerMeter = 0;
		m_bmp_info.info.biClrUsed = 256; // use 256  colors
		m_bmp_info.info.biClrImportant = 0; // all colors important
											//m_bmp.CreateDIBitmap(GetDC(), &m_bmp_info.info,CBM_INIT, &header, 0, NULL, , 256);
											//m_bmp.CreateDIBitmap(GetDC(), &m_bmp_info.info,CBM_INIT, &header, 0, NULL, , 256);
		m_backbuffers[0].CreateDIBitmap(GetDC(), &info->bmiHeader, 0, NULL, info, DIB_RGB_COLORS);
		m_backbuffers[1].CreateDIBitmap(GetDC(), &info->bmiHeader, 0, NULL, info, DIB_RGB_COLORS);
		//m_backbuffers[0].CreateCompatibleBitmap(GetDC(), width, height); // 24 bit agb?
		vid_buffer.resize(this->stride() * this->width());
		back_buffer.resize(this->stride() * this->width());
		zbuffer.resize(this->height() * this->width()); // this right?
		surfcache.resize(256 * 1024);
		buffers_setup = true;
		SetPalette(palette);
	}

	DECLARE_WND_CLASS(_T("My CBitmapWindow Class"))

	BEGIN_MSG_MAP(CBitmapWindow)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		//	MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CDoubleBufferWindowImpl)
	END_MSG_MAP()


	void DoPaint(CDCHandle dc)
	{
		if (buffers_setup)
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(dc);
			HBITMAP hBmpOld = dcMem.SelectBitmap(m_backbuffers[(m_current+1) & 1]);
			dc.StretchBlt(0, 0, 640, 480, dcMem, 0, 0, 320, 240, SRCCOPY);
		//	dc.BitBlt(0, 0, m_size.cx, m_size.cy, dcMem, 0, 0, SRCCOPY);
			dcMem.SelectBitmap(hBmpOld);
		}
	}

	void Update(const uint8_t* pixels) {
		backbuffer()->SetBitmapBits(width()*height(), pixels);
		flipbuffer();
		this->Invalidate();
	}
#if 0
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if (wParam != NULL)
			DoPaint((HDC)wParam);
		else
			DoPaint(CPaintDC(m_hWnd).m_hDC);
		return 0;
		}
#endif
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DestroyWindow();
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}

	// doom interface
	void	VID_Shutdown(void)
	{
	}
	void	VID_Init(unsigned char *palette)
	{
		ATLASSERT(buffers_setup);
		vid.maxwarpwidth = vid.width = vid.conwidth = width();
		vid.maxwarpheight = vid.height = vid.conheight = height();
		vid.aspect = 1.0;
		vid.numpages = 1;
		vid.colormap = host_colormap;
		vid.fullbright = 256 - LittleLong(*((int *)vid.colormap + 2048));
		vid.buffer = vid.conbuffer = vid_buffer.data();
		vid.rowbytes = vid.conrowbytes = stride();

//		d_pzbuffer = zbuffer.data();
		D_InitCaches(surfcache.data(), surfcache.size());





	}
	void VID_Update(vrect_t *rects);
	void D_BeginDirectRect(int x, int y, byte *pbitmap, int width, int height)
	{
		// skip for now, mainly used for the disk icon
	}
	void D_EndDirectRect(int x, int y, int width, int height)
	{
		// skip for now, mainly used for the disk icon
	}
};
extern CBitmapWindow wndMain;
#endif

#if 0
int (PASCAL FAR *pWSAStartup)(WORD wVersionRequired, LPWSADATA lpWSAData);
int (PASCAL FAR *pWSACleanup)(void);
int (PASCAL FAR *pWSAGetLastError)(void);
SOCKET (PASCAL FAR *psocket)(int af, int type, int protocol);
int (PASCAL FAR *pioctlsocket)(SOCKET s, long cmd, u_long FAR *argp);
int (PASCAL FAR *psetsockopt)(SOCKET s, int level, int optname,
							  const char FAR * optval, int optlen);
int (PASCAL FAR *precvfrom)(SOCKET s, char FAR * buf, int len, int flags,
							struct sockaddr FAR *from, int FAR * fromlen);
int (PASCAL FAR *psendto)(SOCKET s, const char FAR * buf, int len, int flags,
						  const struct sockaddr FAR *to, int tolen);
int (PASCAL FAR *pclosesocket)(SOCKET s);
int (PASCAL FAR *pgethostname)(char FAR * name, int namelen);

struct hostent FAR * (PASCAL FAR *pgethostbyname)(const char FAR * name);
struct hostent FAR * (PASCAL FAR *pgethostbyaddr)(const char FAR * addr,
												  int len, int type);
int (PASCAL FAR *pgetsockname)(SOCKET s, struct sockaddr FAR *name,
							   int FAR * namelen);
#endif

#endif