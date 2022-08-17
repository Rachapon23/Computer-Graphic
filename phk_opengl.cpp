#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "phk_opengl.h"

#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x

LRESULT CALLBACK /*_export*/ phkOpenGLDefWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

static char lpszphkOpenGLengine[] = "PHK_OPENGL_WRAPPER";

phkOpenGLengine::phkOpenGLengine()
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_nId = 0;
	m_nDrag = 0;
	m_atom = 0;

	m_hDC = NULL;
	m_hRC = NULL;

	m_px = 0.0f;
	m_py = 0.0f;

	m_rot[0] = 0.0f;
	m_rot[1] = 0.0f;

	m_nmesh = 0;

	m_fDir = 1.0f;

	m_isanimate = 0;
}

void phkOpenGLengine::purge(void)
{
	long    i;

	for (i = 0; i < MAX_SURFACES; i++)
	{
		if (m_nListCreated[i]) ::glDeleteLists(i + 1, 1);
		m_nListCreated[i] = 0;
	}

	if (m_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hRC);
	}
	if (m_hWnd && m_hDC)
	{
		ReleaseDC(m_hWnd, m_hDC);
	}

	m_hDC = NULL;
	m_hRC = NULL;

	//   MessageBox (NULL, "OpenGL context is succesfully cleaned", "Exit OpenGL", MB_OK);
}

phkOpenGLengine::~phkOpenGLengine()
{
	if (m_hWnd) DestroyWindow(m_hWnd);

	m_hWnd = NULL;
	m_hParent = NULL;
	m_nId = 0;
	m_atom = 0;
}

void phkOpenGLengine::registerClass(HINSTANCE hInst)
{
	WNDCLASS     wndclass;

	if (!GetClassInfo(hInst, lpszphkOpenGLengine, &wndclass))
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = (WNDPROC) ::phkOpenGLDefWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = sizeof(phkOpenGLengine *);
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
		wndclass.hbrBackground = (HBRUSH)0;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = lpszphkOpenGLengine;

		m_atom = RegisterClass(&wndclass);
	}
}

void phkOpenGLengine::unregisterClass(HINSTANCE hInst)
{
	WNDCLASS     wndclass;

	if (GetClassInfo(hInst, lpszphkOpenGLengine, &wndclass))
		UnregisterClass(lpszphkOpenGLengine, hInst);
}

void phkOpenGLengine::Create(HWND hwndParent, int id, LPRECT rc)
{
	m_hWnd = CreateWindow(lpszphkOpenGLengine, NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		rc->left, rc->top, rc->right, rc->bottom,
		hwndParent, (HMENU)id, NULL, this);
	m_nId = id;
	m_hParent = hwndParent;

	initopengl();
}


void phkOpenGLengine::resize(int cx, int cy)
{
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();

	::gluPerspective(40, (float)cx / cy, 0.001, 40.0);
	::glViewport(0, 0, cx, cy);

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	::glEnable(GL_DEPTH_TEST);
	::glDepthFunc(GL_LESS);
}

void phkOpenGLengine::drawaxes(void)
{
	//room

	//Add ambient light
	GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f }; //Color(0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	//Add positioned light
	GLfloat lightColor0[] = { 0.5f, 0.5f, 0.5f, 1.0f }; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = { 4.0f, 0.0f, 8.0f, 1.0f }; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	//Add directed light
	GLfloat lightColor1[] = { 0.5f, 0.2f, 0.2f, 1.0f }; //Color (0.5, 0.2, 0.2)
														//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = { -1.0f, 0.5f, 0.5f, 0.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);



	///////////////////////////

	::glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);



	::glBegin(GL_QUAD_STRIP);



	::glColor3f(1, 0.889, 0.710);

	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 0, 1.5);
	glVertex3f(1, 0, 1.5);

	::glEnd();

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.725, 0.726);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 0.5, 0);



	::glEnd();

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.755, 0.726);

	glVertex3f(1, 0, 0);
	glVertex3f(1, 0.5, 0);
	glVertex3f(1, 0, 1.5);
	glVertex3f(1, 0.5, 1.5);

	::glEnd();

	////////////////////////////////////////////////////////////////////
	//PILLOW

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.9, 0, 0.5);
	glVertex3f(0.9, 0, 0.75);
	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.9, 0.095, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.99, 0, 0.5);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.095, 0.5);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.9, 0, 0.5);
	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.99, 0., 0.5);
	glVertex3f(0.99, 0.095, 0.5);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.9, 0, 0.75);
	glVertex3f(0.9, 0.095, 0.75);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.9, 0.095, 0.75);
	glVertex3f(0.99, 0.095, 0.5);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();
	///////////////////////////////////////0.075
	///////////////////////////////////////////////////////////////////

	//bed

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.4, 0.689, 1);

	glVertex3f(0.5, 0, 0.5);
	glVertex3f(0.5, 0, 0.75);
	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.5, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.094, 0.659, 1);

	glVertex3f(0.99, 0, 0.5);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.094, 0.659, 1);

	glVertex3f(0.5, 0, 0.5);
	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.99, 0., 0.5);
	glVertex3f(0.99, 0.075, 0.5);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.4, 0.689, 1);

	glVertex3f(0.5, 0, 0.75);
	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.094, 0.659, 1);

	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();
	///////////////////////////////////////

	//cabinet

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.686, 0.4);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.353, 0.063);

	glVertex3f(0.99, 0, 1.25);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.686, 0.4);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.99, 0., 1.25);
	glVertex3f(0.99, 0.3, 1.25);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.686, 0.4);

	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 0.6, 0.2);

	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();
	///////////////////////////////////////////////////
	//PILLOW D

	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.9, 0, 0.5);
	glVertex3f(0.9, 0, 0.75);
	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.9, 0.095, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.99, 0, 0.5);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.095, 0.5);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.9, 0, 0.5);
	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.99, 0., 0.5);
	glVertex3f(0.99, 0.095, 0.5);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.9, 0, 0.75);
	glVertex3f(0.9, 0.095, 0.75);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.9, 0.095, 0.5);
	glVertex3f(0.9, 0.095, 0.75);
	glVertex3f(0.99, 0.095, 0.5);
	glVertex3f(0.99, 0.095, 0.75);

	::glEnd();
	///////////////////////////////////////


	//cabinet ©

	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.99, 0, 1.25);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.99, 0., 1.25);
	glVertex3f(0.99, 0.3, 1.25);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);
	//
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.25);

	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0.3, 1.45);


	glVertex3f(0.8, 0.3, 1.35);
	glVertex3f(0.8, 0, 1.35);

	::glEnd();

	///////////////////////////////////////////////////
	//bed f

	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.5, 0, 0.5);
	glVertex3f(0.5, 0, 0.75);
	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.5, 0.075, 0.75);

	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.99, 0, 0.5);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.5, 0, 0.5);
	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.99, 0., 0.5);
	glVertex3f(0.99, 0.075, 0.5);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.5, 0, 0.75);
	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0, 0.75);
	glVertex3f(0.99, 0.075, 0.75);

	::glEnd();

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.75);

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.5);

	glVertex3f(0.5, 0.075, 0.5);
	glVertex3f(0.5, 0.075, 0.75);
	glVertex3f(0.99, 0.075, 0.5);
	glVertex3f(0.99, 0.075, 0.75);


	::glEnd();
	///////////////////////////////////////

	//cabinet

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 1, 0.053);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 1, 0.053);

	glVertex3f(0.99, 0, 1.25);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 1, 0.053);

	glVertex3f(0.8, 0, 1.25);
	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.99, 0., 1.25);
	glVertex3f(0.99, 0.3, 1.25);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 1, 0.053);

	glVertex3f(0.8, 0, 1.45);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0, 1.45);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(1, 1, 0.053);

	glVertex3f(0.8, 0.3, 1.25);
	glVertex3f(0.8, 0.3, 1.45);
	glVertex3f(0.99, 0.3, 1.25);
	glVertex3f(0.99, 0.3, 1.45);

	::glEnd();
	///////////////////////////////////////////////////

	//AIR

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.753, 0.753, 0.753);

	glVertex3f(0.35, 0.35, 0);
	glVertex3f(0.35, 0.45, 0);
	glVertex3f(0.35, 0.35, 0.05);
	glVertex3f(0.35, 0.45, 0.05);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.753, 0.753, 0.753);

	glVertex3f(0.65, 0.35, 0);
	glVertex3f(0.65, 0.45, 0);
	glVertex3f(0.65, 0.35, 0.05);
	glVertex3f(0.65, 0.45, 0.05);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.753, 0.753, 0.753);

	glVertex3f(0.35, 0.35, 0);
	glVertex3f(0.35, 0.35, 0.05);
	glVertex3f(0.65, 0.35, 0);
	glVertex3f(0.65, 0.35, 0.05);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(139.0f, 69.0f, 19.0f);

	glVertex3f(0.35, 0.35, 0.05);
	glVertex3f(0.35, 0.45, 0.05);
	glVertex3f(0.65, 0.35, 0.05);
	glVertex3f(0.65, 0.45, 0.05);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.753, 0.753, 0.753);

	glVertex3f(0.35, 0.45, 0);
	glVertex3f(0.35, 0.45, 0.05);
	glVertex3f(0.65, 0.45, 0);
	glVertex3f(0.65, 0.45, 0.05);

	::glEnd();
	///////////////////////////////////////

	//AIR ©

	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.35, 0.4, 0.05);
	glVertex3f(0.65, 0.4, 0.05);

	glVertex3f(0.35, 0.375, 0.05);
	glVertex3f(0.65, 0.375, 0.05);

	glVertex3f(0.35, 0.425, 0.05);
	glVertex3f(0.65, 0.425, 0.05);

	glVertex3f(0.35, 0.45, 0.05);
	glVertex3f(0.65, 0.45, 0.05);

	glVertex3f(0.35, 0.35, 0.05);
	glVertex3f(0.65, 0.35, 0.05);

	glVertex3f(0.35, 0.35, 0.05);
	glVertex3f(0.35, 0.45, 0.05);

	glVertex3f(0.65, 0.35, 0.05);
	glVertex3f(0.65, 0.45, 0.05);




	::glEnd();

	///////////////////////////////////////

	//WINDOW


	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.631, 0.839, 1);

	glVertex3f(0.995, 0.25, 0.5);
	glVertex3f(0.995, 0.4, 0.5);
	glVertex3f(0.995, 0.25, 0.75);
	glVertex3f(0.995, 0.4, 0.75);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.631, 0.839, 1);

	glVertex3f(1.001, 0.25, 0.5);
	glVertex3f(1.001, 0.4, 0.5);
	glVertex3f(1.001, 0.25, 0.75);
	glVertex3f(1.001, 0.4, 0.75);

	::glEnd();


	///////////////////////////////////////

	//WINDOW ©


	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.995, 0.25, 0.5);
	glVertex3f(0.995, 0.4, 0.5);
	glVertex3f(0.995, 0.25, 0.75);
	glVertex3f(0.995, 0.4, 0.75);

	glVertex3f(0.995, 0.25, 0.5);
	glVertex3f(0.995, 0.25, 0.75);

	glVertex3f(0.995, 0.4, 0.5);
	glVertex3f(0.995, 0.4, 0.75);

	//
	glVertex3f(0.995, 0.4, 0.625);
	glVertex3f(0.995, 0.25, 0.625);

	glVertex3f(0.995, 0.325, 0.5);
	glVertex3f(0.995, 0.325, 0.75);


	::glEnd();

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(1.001, 0.25, 0.5);
	glVertex3f(1.001, 0.4, 0.5);
	glVertex3f(1.001, 0.25, 0.75);
	glVertex3f(1.001, 0.4, 0.75);

	glVertex3f(1.001, 0.25, 0.5);
	glVertex3f(1.001, 0.25, 0.75);

	glVertex3f(1.001, 0.4, 0.5);
	glVertex3f(1.001, 0.4, 0.75);

	glVertex3f(1.001, 0.4, 0.625);
	glVertex3f(1.001, 0.25, 0.625);

	glVertex3f(1.001, 0.325, 0.5);
	glVertex3f(1.001, 0.325, 0.75);

	::glEnd();


	///////////////////////////////////////

	//TABLE

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.826, 0.490, 0.055);

	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0, 0.75);
	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.627, 0.321, 0.019);

	glVertex3f(0.1, 0, 0.5);
	glVertex3f(0.1, 0, 0.75);
	glVertex3f(0.1, 0.075, 0.5);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.826, 0.490, 0.055);

	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0.1, 0., 0.5);
	glVertex3f(0.1, 0.075, 0.5);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.826, 0.490, 0.055);

	glVertex3f(0, 0, 0.75);
	glVertex3f(0, 0.075, 0.75);
	glVertex3f(0.1, 0, 0.75);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0.627, 0.321, 0.019);

	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0, 0.075, 0.75);
	glVertex3f(0.1, 0.075, 0.5);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();
	///////////////////////////////////////
	//TABLE HH

	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0, 0.075, 0.75);
	glVertex3f(0.1, 0.075, 0.75);

	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0.1, 0.075, 0.5);

	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0, 0.75);
	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0.1, 0, 0.5);
	glVertex3f(0.1, 0, 0.75);
	glVertex3f(0.1, 0.075, 0.5);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0.1, 0., 0.5);
	glVertex3f(0.1, 0.075, 0.5);

	::glEnd();
	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0, 0, 0.75);
	glVertex3f(0, 0.075, 0.75);
	glVertex3f(0.1, 0, 0.75);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();

	//
	::glBegin(GL_LINES);

	::glColor3f(0, 0, 0);

	glVertex3f(0, 0.075, 0.5);
	glVertex3f(0, 0.075, 0.75);
	glVertex3f(0.1, 0.075, 0.5);
	glVertex3f(0.1, 0.075, 0.75);

	::glEnd();
	///////////////////////////////////////

	//TV

	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0, 0, 0);

	glVertex3f(0.025, 0.075, 0.5);
	glVertex3f(0.025, 0.075, 0.75);
	glVertex3f(0.025, 0.175, 0.5);
	glVertex3f(0.025, 0.175, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0, 0, 0);

	glVertex3f(0.03, 0.075, 0.5);
	glVertex3f(0.03, 0.075, 0.75);
	glVertex3f(0.03, 0.175, 0.5);
	glVertex3f(0.03, 0.175, 0.75);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0, 0, 0);

	glVertex3f(0.025, 0.075, 0.5);
	glVertex3f(0.025, 0.175, 0.5);
	glVertex3f(0.03, 0.075, 0.5);
	glVertex3f(0.03, 0.175, 0.5);

	::glEnd();
	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0, 0, 0);

	glVertex3f(0.025, 0.075, 0.75);
	glVertex3f(0.025, 0.175, 0.75);
	glVertex3f(0.03, 0.075, 0.75);
	glVertex3f(0.03, 0.175, 0.75);

	::glEnd();

	//
	::glBegin(GL_QUAD_STRIP);

	::glColor3f(0, 0, 0);

	glVertex3f(0.025, 0.175, 0.5);
	glVertex3f(0.025, 0.175, 0.75);
	glVertex3f(0.03, 0.175, 0.5);
	glVertex3f(0.03, 0.175, 0.75);

	::glEnd();
	///////////////////////////////////////




	::glColor3f(1.0f, 1.0f, 1.0f);
	::glEnable(GL_LIGHTING);


}
void phkOpenGLengine::mouse_update(float cx, float cy)
{
	m_rot[0] += ((cy - m_py) * 0.1f);
	m_rot[1] -= ((cx - m_px) * 0.1f);

	m_px = cx;
	m_py = cy;

	clamp(m_rot[0]);
	clamp(m_rot[1]);
}

void phkOpenGLengine::recoverRigidDisplay(void)
{
	::glTranslatef(-0.5f, -0.5f, -3.00f);
	::glRotatef(m_rot[0], 1.0f, 0.0f, 0.0f);
	::glRotatef(m_rot[1], 0.0f, 1.0f, 0.0f);
//	::glRotatef(1.2f, 0.0f, 0.4f, 0.5f);
}

void phkOpenGLengine::display(void)
{
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	::glPushMatrix();

	recoverRigidDisplay();

	drawaxes();

	::glFlush();
	::glPopMatrix();

	::SwapBuffers(m_hDC);          /* nop if singlebuffered */
}

void phkOpenGLengine::initopengl(void)
{
	PIXELFORMATDESCRIPTOR   pfd;
	int                     format;
	RECT                    rcclient;

	m_hDC = GetDC(m_hWnd);

	// set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	format = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, format, &pfd);

	// create the render context (RC)
	m_hRC = wglCreateContext(m_hDC);

	// make it the current render context
	wglMakeCurrent(m_hDC, m_hRC);

	GetClientRect(m_hWnd, &rcclient);

	//  ::glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	resize(rcclient.right, rcclient.bottom);
	initlighting();
}

void phkOpenGLengine::initlighting(void)
{
	//  initalize light
	GLfloat ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat position0[] = { -0.2f, 0.5f, +5.0f, 0.0f };
	GLfloat position1[] = { -0.2f, 0.5f, -5.0f, 0.0f };

	GLfloat materialShininess[1] = { 8.0f };

	// enable all the lighting & depth effects
	::glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	::glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	::glLightfv(GL_LIGHT0, GL_POSITION, position0);

	::glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	::glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	::glLightfv(GL_LIGHT1, GL_POSITION, position1);

	::glShadeModel(GL_FLAT);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	::glEnable(GL_LIGHTING);
	::glEnable(GL_LIGHT0);
	::glEnable(GL_LIGHT1);

	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glEnable(GL_LINE_SMOOTH);
}

void phkOpenGLengine::getscreenshot(unsigned char *lpimage, int cX, int cY)
{
	::glReadPixels(0, 0, cX, cY, GL_RGB, GL_UNSIGNED_BYTE, lpimage);
}

LRESULT phkOpenGLengine::WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//  static PAINTSTRUCT ps;

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_TIMER:	if (m_isanimate)
	{
		m_rot[0] += 1.0f*(rand() / RAND_MAX) - 0.5f;
		m_rot[1] -= 1.0f*(rand() / RAND_MAX) - 0.5f;

		clamp(m_rot[0]);
		clamp(m_rot[1]);
		display();
	}
					break;

	case WM_PAINT:   display();
		break;

	case WM_SIZE:   resize(LOWORD(lParam), HIWORD(lParam));
		PostMessage(m_hWnd, WM_PAINT, 0, 0);
		break;

	case WM_LBUTTONDOWN:   SetCapture(m_hWnd);
		m_px = (float)LOWORD(lParam);
		m_py = (float)HIWORD(lParam);
		m_nDrag = 1;
		break;

	case WM_LBUTTONUP:   ReleaseCapture();
		m_px = 0.0f;
		m_py = 0.0f;
		m_nDrag = 0;
		break;

	case WM_MOUSEMOVE:   if (m_nDrag)
	{
		int mx, my;

		mx = LOWORD(lParam);
		my = HIWORD(lParam);

		if (mx & (1 << 15)) mx -= (1 << 16);
		if (my & (1 << 15)) my -= (1 << 16);

		mouse_update((float)mx, (float)my);
		display();
	}
						 break;

	case WM_DESTROY:   purge();
		break;
	default:
		return DefWindowProc(m_hWnd, iMessage, wParam, lParam);
	}

	return 0;
}

// If data pointers are near pointers
#if defined(__SMALL__) || defined(__MEDIUM__)
inline phkOpenGLengine *GetPointer(HWND hWnd)
{
	return (phkOpenGLengine *)GetWindowWord(hWnd, 0);
}
inline void SetPointer(HWND hWnd, phkOpenGLengine *pOpenGL)
{
	SetWindowWord(hWnd, 0, (WORD)pOpenGL);
}

// else pointers are far
#elif defined(__LARGE__) || defined(__COMPACT__) || defined(__FLAT__)
inline phkOpenGLengine *GetPointer(HWND hWnd)
{
	return (phkOpenGLengine *)GetWindowLong(hWnd, 0);
}

inline void SetPointer(HWND hWnd, phkOpenGLengine *pOpenGL)
{
	SetWindowLong(hWnd, 0, (LONG)pOpenGL);
}

//#else
////    #error Choose another memory model!
#endif

LRESULT FAR PASCAL /*_export*/ phkOpenGLDefWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	phkOpenGLengine *pOpenGL = GetPointer(hwnd);

	switch (message)
	{
	case WM_CREATE:   if (!pOpenGL)
	{
		LPCREATESTRUCT lpcs;

		lpcs = (LPCREATESTRUCT)lParam;
		pOpenGL = (phkOpenGLengine *)lpcs->lpCreateParams;

		// Store a pointer to this object in the window's extra bytes;
		// this will enable us to access this object (and its member
		// functions) in WndProc where we are
		// given only a handle to identify the window.
		SetPointer(hwnd, pOpenGL);
		// Now let the object perform whatever
		// initialization it needs for WM_CREATE in its own
		// WndProc.
		return pOpenGL->WndProc(message, wParam, lParam);
	}
					  break;

	default:   if (pOpenGL) pOpenGL->WndProc(message, wParam, lParam);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

