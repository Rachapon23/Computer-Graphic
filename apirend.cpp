#define STRICT
#include <windows.h>
#pragma hdrstop
#include <windowsx.h>
#include <io.h>

#include "resource.h"

#include "phk_opengl.h"
#include "apirend.h"

static HINSTANCE        m_hInstance     = NULL;
static phkOpenGLengine  m_opengl;
LRESULT FAR PASCAL		WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void* api_malloc (unsigned long size)
{
    return ::GlobalAlloc (GMEM_FIXED, size);
}

void* api_calloc (unsigned long size)
{
    return ::GlobalAlloc (GPTR, size);
}

void api_free (void* fixedptr)
{
    if (fixedptr) ::GlobalFree ((HGLOBAL) fixedptr);
}
void    RegisterChildWindows (HINSTANCE hInst)
{
    m_opengl.registerClass (hInst);
}

void    UnRegisterChildWindows (HINSTANCE hInst)
{
    m_opengl.unregisterClass (hInst);
}

void    CreateChildWindows (HWND hwnd, HINSTANCE hInst)
{
    RECT    rcopengl    = {   5,    5, 1004, 682};

    m_opengl.Create (hwnd, IDC_OPENGL, &rcopengl);
}

int DrawChildWindows (int idCtl, const DRAWITEMSTRUCT FAR * lpdist)
{
    int ret = 0;

    return ret;
}

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd,
                          int nCmdShow)
{
    static char szAppName[] = "523652: Advanced Computer Graphics" ;
    HWND        hwnd ;
    MSG         msg ;
    WNDCLASS        wndclass ;
    HANDLE      hAccel;

    if (!hPrevInstance)
    {
        wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
        wndclass.lpfnWndProc   = (WNDPROC) WndProc ;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = hInstance ;
        wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
        wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
        wndclass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
        wndclass.lpszMenuName  = MAKEINTRESOURCE (PROGRAM_MENU);
        wndclass.lpszClassName = szAppName ;

        RegisterClass (&wndclass);
    }

    RegisterChildWindows (hInstance);

    hwnd = CreateWindow (szAppName,
                        "523652: Advanced Computer Graphics",
                        WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU & ~WS_THICKFRAME,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        1024,
                        720,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ShowWindow (hwnd, nCmdShow);
    UpdateWindow (hwnd);

    hAccel      = LoadAccelerators( hInstance, MAKEINTRESOURCE (PROGRAM_ACCEL));
    m_hInstance = hInstance;

    while (GetMessage (&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator (hwnd, (HACCEL) hAccel, &msg))
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }

    UnRegisterChildWindows (hInstance);

    return msg.wParam;
}

LRESULT FAR PASCAL WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int             ret;
	long			aa [2] = { 10, 13};

    switch (message)
    {   
        case WM_CREATE      :   CreateChildWindows (hwnd, m_hInstance);
                                break;

        case WM_DRAWITEM    :   ret = DrawChildWindows ((int) wParam, (const DRAWITEMSTRUCT FAR *) lParam);
                                return ret;
        
        case WM_COMMAND     :

        switch (GET_WM_COMMAND_ID (wParam, lParam))
        {
            case MIC_U_HELPABOUT    :   MessageBox (hwnd, "523652: Advanced Computer Graphics 1.0\n\nCopyright (c) 2004 P. Horkaew",
                                                    "About Program", MB_ICONINFORMATION );
                                        break;

            case MIC_U_EXIT         :   DestroyWindow( hwnd );
                                        break;

        }
        break;

        case WM_TIMER   :
                            break;

        case WM_DESTROY :   SendMessage (m_opengl.GetHandle (), WM_DESTROY, 0, 0);
                            PostQuitMessage (0);
                            return 0 ;
   }

   return DefWindowProc (hwnd, message, wParam, lParam);
}
