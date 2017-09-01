#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlscrl.h>
typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	WS_EX_APPWINDOW> CMyWindowTraits;

class CBitmapWindow : public CWindowImpl<CBitmapWindow, CWindow, CMyWindowTraits>
{
public:
	CBitmap m_bmp;
	SIZE m_size;
	DECLARE_WND_CLASS(_T("My CBitmapWindow Class"))

	BEGIN_MSG_MAP(CBitmapWindow)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	void DoPaint(CDCHandle dc)
	{
		if (!m_bmp.IsNull())
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(dc);
			HBITMAP hBmpOld = dcMem.SelectBitmap(m_bmp);
			dc.BitBlt(0, 0, m_size.cx, m_size.cy, dcMem, 0, 0, SRCCOPY);
			dcMem.SelectBitmap(hBmpOld);
		}
	}
	void CreateBitmap(size_t width, size_t height) {
		m_size.cx = width;
		m_size.cy = height;
		CPaintDC dc(m_hWnd);
		m_bmp.CreateCompatibleBitmap(dc, width, height);
	}
	void Update(const uint32_t* pixels) {
		m_bmp.SetBitmapBits(m_size.cx*m_size.cy, pixels);
		this->Invalidate();
	}
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if (wParam != NULL)
			DoPaint((HDC)wParam);
		else
			DoPaint(CPaintDC(m_hWnd).m_hDC);
		return 0;
	}

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

};
