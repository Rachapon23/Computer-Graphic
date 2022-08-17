#ifndef	OPENGL_WINDOW_WRAPPER
#define	OPENGL_WINDOW_WRAPPER

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
//#include <gl\glut.h>

#include "apirend.h"

#define	IDC_OPENGLTIMER		0x11A5

class	phkOpenGLengine
{
public:
	 phkOpenGLengine ();
   ~phkOpenGLengine ();

public:
	void	registerClass (HINSTANCE hInst);
	void	unregisterClass (HINSTANCE hInst);
	void	Create (HWND hwndParent, int id, LPRECT rc);
	void	initopengl (void);
	void	purge (void);
	HWND	GetHandle (void) { return m_hWnd; }	

	LRESULT		WndProc (UINT iMessage, WPARAM wParam, LPARAM lParam);
	void		display (void);
	void		getscreenshot (unsigned char *lpimage, int cX, int cY);

protected:
	void	resize (int cx, int cy);
	void	mouse_update (float cx, float cy);
	void	initlighting (void);
	void	recoverRigidDisplay (void);
	void	drawaxes (void);

protected:
	HWND		m_hWnd, m_hParent;
	int			m_nId, m_nDrag;
	ATOM		m_atom;
	HDC			m_hDC;
	HGLRC		m_hRC;
	float		m_rot [2], m_px, m_py;
	
	int			m_nListCreated [MAX_SURFACES];
	long		m_nmesh;

	float		m_fDir;

	int			m_isanimate;
};

#endif
