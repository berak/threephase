#include "glapp.h"


void reshape(int w, int h) 
{
 	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);  
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

int main()
//int WINAPI WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int iCmdShow)
{
	HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;

    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "asmwarp";
    RegisterClass (&wc);

    hWnd = CreateWindow (
      "asmwarp", "ThreePhase", 
	  WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      0, 0, 400, 400,
      NULL, NULL, hInstance, NULL);

    EnableOpenGL (hWnd, &hDC, &hRC);

	appInit();

	while (!bQuit)
    {
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
			appIdle();
		    SwapBuffers (hDC);
			Sleep (1);
        }
    }
    DisableOpenGL (hWnd, hDC, hRC);
    DestroyWindow (hWnd);
    return msg.wParam;
}


LRESULT CALLBACK 
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;
	case WM_CHAR:
//		keyboard(wParam,0,0);
		break;
    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}


void
EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    *hDC = GetDC (hWnd);

	ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

	*hRC = wglCreateContext( *hDC );
    BOOL r = wglMakeCurrent( *hDC, *hRC );

}


void
DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

