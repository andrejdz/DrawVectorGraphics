#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <strsafe.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnDestroy(HWND);
void WndProc_OnPaint(HWND);

LPTSTR g_lpszClassName = TEXT("DrawVectorGraphics");
LPTSTR g_lpszApplicationTitle = TEXT("Developer: Dyagel Andrew");
LPCTSTR g_fileEMFPath = TEXT("MyAppMetaFile.emf");

HDC g_hdcEMF;
HENHMETAFILE g_hemf;

int APIENTRY _tWinMain(HINSTANCE This,
					   HINSTANCE hPrevInstance,
					   LPTSTR lpszCmdLine,
					   int mode)
{
	HWND hWnd;
	MSG msg;
	WNDCLASSEX wc;

	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = This;
	wc.lpszClassName = g_lpszClassName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDC_ICON);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wc.hIconSm = NULL;

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, TEXT("Error register window class!"),
				   TEXT("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindowEx(NULL,
						  g_lpszClassName,
						  g_lpszApplicationTitle,
						  WS_OVERLAPPEDWINDOW,
						  350,
						  200,
						  500,
						  500,
						  HWND_DESKTOP,
						  NULL,
						  This,
						  NULL);
	if(!hWnd)
	{
		MessageBox(NULL, TEXT("Can't create window!"),
				   TEXT("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, mode);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, WndProc_OnPaint);
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

BOOL WndProc_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	g_hdcEMF = CreateEnhMetaFile(NULL, g_fileEMFPath, NULL, NULL);
	if(!g_hdcEMF)
	{
		return -1;
	}

	size_t sizeBuffer = 32;
	LPTSTR buffer = (LPTSTR)calloc(sizeBuffer, sizeof(wchar_t));
	if(buffer == NULL)
	{
		MessageBox(hwnd, TEXT("Can not allocate memory!"), TEXT("Error"), MB_OK);
	}
	buffer = TEXT("Arial");

	size_t sizeToMessage = 100;
	LPCTSTR lpcszMessagePaint = (LPCTSTR)calloc(sizeToMessage, sizeof(wchar_t));
	if(lpcszMessagePaint == NULL)
	{
		MessageBox(hwnd, TEXT("Can not allocate memory!"), TEXT("Error"), MB_OK);
	}
	lpcszMessagePaint = TEXT("Andrew Dyagel");

	HFONT hFont = CreateFont(-MulDiv(28, GetDeviceCaps(g_hdcEMF, LOGPIXELSY), 72),
							 0, 0, 0, 700, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
							 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_DONTCARE, buffer);

	SelectObject(g_hdcEMF, hFont);
	SetTextColor(g_hdcEMF, RGB(0, 100, 0));

	TextOut(g_hdcEMF, 80, 300, lpcszMessagePaint, _tcsclen(lpcszMessagePaint));

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 128, 0));
	SelectObject(g_hdcEMF, hPen);

	HBRUSH hBrushRect = CreateSolidBrush(RGB(0, 0, 128));
	SelectObject(g_hdcEMF, hBrushRect);
	Rectangle(g_hdcEMF, 200, 100, 260, 220);

	HBRUSH hBrushElCross = CreateHatchBrush(HS_CROSS, RGB(0, 0, 128));
	SelectObject(g_hdcEMF, hBrushElCross);
	Pie(g_hdcEMF, 80, 100, 320, 220, 200, 100, 200, 220);

	HBRUSH hBrushElDiagCross = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 128));
	SelectObject(g_hdcEMF, hBrushElDiagCross);
	Pie(g_hdcEMF, 140, 100, 380, 220, 260, 220, 260, 100);

	DeleteObject(hFont);
	DeleteObject(hPen);
	DeleteObject(hBrushRect);
	DeleteObject(hBrushElCross);
	DeleteObject(hBrushElDiagCross);

	CloseEnhMetaFile(g_hdcEMF);

	FORWARD_WM_CREATE(hwnd, lpCreateStruct, DefWindowProc);
	return TRUE;
}

void WndProc_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	RECT rect = {80, 100, 380, 328};

	if(g_hemf = GetEnhMetaFile(g_fileEMFPath))
	{
		PlayEnhMetaFile(hdc, g_hemf, &rect);
		DeleteEnhMetaFile(g_hemf);
	}

	EndPaint(hwnd, &ps);
	FORWARD_WM_PAINT(hwnd, DefWindowProc);
}

void WndProc_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
	FORWARD_WM_DESTROY(hwnd, DefWindowProc);
}