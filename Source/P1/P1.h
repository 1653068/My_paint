#pragma once

#include "resource.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include <ColorDlg.h>
#include "Object.h"
#include <vector>
#include <Winuser.h>

#define ID_TOOLBAR 1000
#define MAX_LOADSTRING 100
#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    17
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0
#define TOOL_TIP_MAX_LEN   32

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWndClient, hWndFrame, hWndToolbar;
int New = 1;
WCHAR OpenFileName[255], SaveFileName[255];
WCHAR szFile[260]; //File Name
int FlagOpen = 0;
HDC memDC, hdc;
HBITMAP hBitmap;

enum DrawMode
{
	MD_LINE, MD_RECTANGLE, MD_ELLIPSE, MD_TEXT, MD_SELECT
};

enum EditBox
{
	MOUSEUP, MOUSECLICK, NONE, MOUSEMOVE
};

CHOOSECOLOR cc;
RECT Mouseprev;
Object *tmp;

struct CHILD_WND_DATA
{
	HWND hWnd, editbox;
	vector <Object*> Ob;
	COLORREF Color;
	LOGFONT Font;
	WCHAR szFile[260];
	int Click_EditBox, Clicked;
	int IndexSelect;
};

struct ObjectLineRectEll
{
	RECT Size;
	COLORREF Color;
	int Type;
};

struct ObjectText
{
	RECT Size;
	COLORREF Color;
	LOGFONT Font;
	WCHAR _Text[256];
	int len;
};

RECT Mouse, ChildRec;

int ChooseMode = MD_LINE, FlagInit = 0;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    MDICloseWndProc(HWND, LPARAM);

//UI - MDI
void initFrameWnd(HWND hWnd);
void onCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void onNewWnd(HWND hWnd, CHILD_WND_DATA *OpenData);
void initChildWindow(HWND hWnd);
void onMDIActivate(HWND hWnd);
void DrawAgain(HWND hWnd);
//Common Dialog
void onColorDlg(HWND hWnd);
void onFontDlg();
void onOpenDlg(HWND hWnd, CHILD_WND_DATA *&OpenData);
void onSaveAsDlg();
//Edit command 
void onCut();
void onCopy();
void onPaste();
void onDelete();
//Message Mouse
void OnMouseClick(HWND hWnd, LPARAM lParam, RECT &Mouse, int &x, int &y);
void OnMouseMove(HWND hWnd, LPARAM lParam, WPARAM wParam, RECT &Mouse, int &x, int &y);
void OnMouseUp(HWND hWnd, LPARAM lParam, RECT &Mouse);
//Toolbar
void doCreate_ToolBar();
void doToolBar_AddUserButton();
