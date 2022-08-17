#ifndef	MAIN_FUNCTIONS_DECLARATION
#define	MAIN_FUNCTIONS_DECLARATION

#include <windows.h>

//	functions defined in apirend.cpp
void*	api_malloc (unsigned long size);
void*	api_calloc (unsigned long size);
void	api_free (void* fixedptr);

void	RegisterChildWindows (HINSTANCE hInst);
void	UnRegisterChildWindows (HINSTANCE hInst);
void	CreateChildWindows (HWND hwnd, HINSTANCE hInst);
int		DrawChildWindows (int idCtl, const DRAWITEMSTRUCT FAR * lpdist);

#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#define FABS(x)		((x) >= 0 ? (x) : -(x))

#define	MAX_SURFACES		4
#define	IMAGE_SCALEF		1.00f
#define	MSCALE				0.60f
#define	SAFE_GROUND			4		//	safe 0th and last slice offset location (4 pixels)

#define	MR_PIXELSIZE		(1.0f/24.0f)

#endif
