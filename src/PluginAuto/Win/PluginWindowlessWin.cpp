#include "PluginWindowlessWin.h"
#include "ConstructDefaultPluginWindows.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

using namespace std; using namespace FB;

FB::PluginWindowlessWin* FB::createPluginWindowless(const FB::WindowContextWindowless& ctx)
{
    return new PluginWindowlessWin(ctx);
}

PluginWindowlessWin::PluginWindowlessWin(const FB::WindowContextWindowless& ctx)
    : m_hdc(ctx.drawable), m_browserHwnd(NULL) {}

PluginWindowlessWin::~PluginWindowlessWin() {}

void PluginWindowlessWin::translateWindowToPlugin(long &x, long &y) const {
    long tempX, tempY;
    tempX = x - m_x;
    tempY = y - m_y;
    x = tempX;
    y = tempY;
}

bool PluginWindowlessWin::HandleEvent(uint32_t event, uint32_t wParam, uint32_t lParam, LRESULT& lRes)
{ 
    {
        LRESULT lres(0);
        WindowsEvent nEvt((HWND)NULL, event, wParam, lParam, lres);
        if (SendEvent(&nEvt)) {
            lRes = nEvt.lRes;
            return TRUE;
        }
    }
    switch(event) {
        case WM_LBUTTONDOWN: 
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONDOWN:
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONDOWN:
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_LBUTTONUP: 
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONUP:
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONUP:
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_MOUSEMOVE:
        {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseMoveEvent ev(x, y);
            return SendEvent(&ev);
        }
        case WM_PAINT:
        {
            HDC dc = (HDC)wParam;
            if(dc != m_hdc) {
                setHDC(dc);
            }
            RefreshEvent ev;
            if (!SendEvent(&ev)) {
                FB::Rect pos = getWindowPosition();
            	::SetTextAlign(dc, TA_CENTER|TA_BASELINE);
            	LPCTSTR pszText = _T("FireBreath Plugin");
            	::TextOut(dc, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));
            }
            return 0;
        }
        case WM_KEYUP:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyUpEvent ev(fb_key, (unsigned int)wParam);
            return SendEvent(&ev);
        }
        case WM_KEYDOWN:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyDownEvent ev(fb_key, (unsigned int)wParam);
            return SendEvent(&ev);
        }   
    }
    return 0;
}

FB::Rect PluginWindowlessWin::getWindowPosition() const {
    long top = m_y;
    long left = m_x;
    long bottom = m_y + m_height;
    long right = m_x + m_width;
    FB::Rect r = { top, left, bottom, right };
    return r;
}

void PluginWindowlessWin::setWindowPosition(long x, long y, long width, long height) {
    m_x = x;
    m_y = y;
    m_height = height;
    m_width = width;
}

void PluginWindowlessWin::setWindowPosition(FB::Rect pos) {
    m_x = pos.left;
    m_y = pos.top;
    m_height = pos.bottom - m_y;
    m_width = pos.right - m_x;
}

FB::Rect FB::PluginWindowlessWin::getWindowClipping() const {
    FB::Rect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

void PluginWindowlessWin::setWindowClipping(long top, long left, long bottom, long right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

void PluginWindowlessWin::setWindowClipping(FB::Rect clip) {
    m_clipTop = clip.top;
    m_clipLeft = clip.left;
    m_clipBottom = clip.bottom;
    m_clipRight = clip.right;
}

void FB::PluginWindowlessWin::InvalidateWindow() const
{
    if (m_invalidateWindow)
        m_invalidateWindow(0, 0, getWindowWidth(), getWindowHeight());
}
