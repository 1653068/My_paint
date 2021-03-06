// P1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "P1.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_P1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	wcscpy_s(OpenFileName, L"");
	wcscpy_s(SaveFileName, L"");
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_P1));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateMDISysAccel(hWndClient, &msg) &
			!TranslateAccelerator(hWndFrame, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

bool WriteFileWnd(TCHAR* szFile, CHILD_WND_DATA *SaveData) {
	fstream of;
	of.open(szFile, ios::out | ios::binary);

	if (SaveData->Ob.empty())
		return false;
	of.write((char*)&SaveData->Color, sizeof(COLORREF));
	of.write((char*)&SaveData->Font, sizeof(LOGFONT));
	int s = SaveData->Ob.size();
	of.write((char*)&s, sizeof(int));
	for (int i = 0; i < s; i++) {
		SaveData->Ob[i]->Write(of);
	}
	of.close();
	return true;
}

void ReadFileWnd(fstream &iof, CHILD_WND_DATA *&OpenData) {
	int n, type;
	OpenData = new CHILD_WND_DATA;
	iof.read((char*)&OpenData->Color, sizeof(COLORREF));
	iof.read((char*)&OpenData->Font, sizeof(LOGFONT));
	iof.read((char*)&n, sizeof(int));
	for (int i = 0; i < n; i++)
	{
		iof.read((char*)&type, sizeof(int));
		Object *ob;
		switch (type) {
		case MD_LINE:
			ob = new ObjLine;
			ob->Read(iof);
			OpenData->Ob.push_back(ob);
			break;
		case MD_RECTANGLE:
			ob = new ObjRectangle;
			ob->Read(iof);
			OpenData->Ob.push_back(ob);
			break;
		case MD_ELLIPSE:
			ob = new ObjEllipse;
			ob->Read(iof);
			OpenData->Ob.push_back(ob);;
			break;
		case MD_TEXT:
			ob = new ObjText;
			ob->Read(iof);
			OpenData->Ob.push_back(ob);
			break;
		}
	}
	return;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	// Frame Window
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_P1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_P1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	//Child Window
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ChildWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 8;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_P1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"NEWWINDOW";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);
	return 1;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

void OnCheckItem(HWND hWnd, int pos)
{
	for (int i = 2; i < 7; i++)
	{
		if (i != pos)
			CheckMenuItem(GetSubMenu(GetMenu(hWnd), 2), i, MF_UNCHECKED | MF_BYPOSITION);
		else
		{
			CheckMenuItem(GetSubMenu(GetMenu(hWnd), 2), i, MF_CHECKED | MF_BYPOSITION);
			ChooseMode = i - 2;
		}
	}
}

bool SeparateFileName(WCHAR inFile[260], WCHAR outFile[255])
{
	int Length = wcslen(inFile), i = Length - 1, j = 0;
	while (i > 0 && inFile[i] != L'\\')
	{
		outFile[j++] = inFile[i--];
	}
	outFile[j] = L'\0';
	_wcsrev(outFile);
	if (wcswcs(outFile, L".drw") == NULL)
		return false;
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		initFrameWnd(hWnd);
		doCreate_ToolBar();
		doToolBar_AddUserButton();
		return 0;
	case WM_COMMAND:
		onCommand(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hd = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		RECT rFrame, rTool;
		GetClientRect(hWnd, &rFrame);
		MoveWindow(hWndToolbar, 0, 0, rFrame.right, 22, TRUE);
		GetClientRect(hWndToolbar, &rTool);
		SetWindowPos(hWndClient, NULL, 0, rTool.bottom + 2, rFrame.right - rFrame.left, rFrame.bottom - rTool.bottom, SWP_NOZORDER);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefFrameProc(hWnd, hWndClient, message, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int x, y;
	CHILD_WND_DATA *wndData;
	switch (message)
	{
	case WM_GETTITLEBARINFOEX:
		break;
	case WM_LBUTTONDOWN:
		OnMouseClick(hWnd, lParam, Mouse, x, y);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, lParam, wParam, Mouse, x, y);
		break;
	case WM_LBUTTONUP:
		OnMouseUp(hWnd, lParam, Mouse);
		break;
	case WM_CREATE:
		initChildWindow(hWnd);
		break;
	case WM_CLOSE:
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &ChildRec);
		break;
	case WM_MDIACTIVATE:
		onMDIActivate(hWnd);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hd = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &ChildRec);
		wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);
		hdc = GetDC(hWnd);
		memDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, ChildRec.right, ChildRec.bottom);
		SelectObject(memDC, hBitmap);
		FillRect(memDC, &ChildRec, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawAgain(hWnd);
		BitBlt(hdc, 0, 0, ChildRec.right, ChildRec.bottom, memDC, 0, 0, SRCCOPY);
		DeleteDC(memDC);
		DeleteObject(hBitmap);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		break;
	}
	return DefMDIChildProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK    MDICloseWndProc(HWND hWnd, LPARAM lParam)
{
	SendMessage(hWndClient, WM_MDIDESTROY, (WPARAM)hWnd, 0l);
	New = 1;
	return 1;
}


// Frame Window
void initFrameWnd(HWND hWnd)
{
	hWndFrame = hWnd;
	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 2);
	ccs.idFirstChild = 50000;
	hWndClient = CreateWindow(L"MDICLIENT", (LPCTSTR)NULL, WS_CHILD | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)NULL, hInst, (LPVOID)&ccs);
	ShowWindow(hWndClient, SW_SHOW);
	HMENU hMenu = GetMenu(hWnd);
	EnableMenuItem(hMenu, 1, MF_GRAYED | MF_BYPOSITION);
	EnableMenuItem(hMenu, 2, MF_GRAYED | MF_BYPOSITION);
}

void onCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wndID = LOWORD(wParam);
	CHILD_WND_DATA *OpenData = NULL;
	switch (wndID)
	{
	case ID_FILE_NEW:
		onNewWnd(hWnd, OpenData);
		OnCheckItem(hWnd, 2);
		break;
	case ID_FILE_OPEN:
		onOpenDlg(hWnd, OpenData);
		break;
	case ID_FILE_SAVE:
		onSaveAsDlg();
		break;
	case ID_EDIT_CUT:
		onCut();
		break;
	case ID_EDIT_COPY:
		onCopy();
		break;
	case ID_EDIT_PASTE:
		onPaste();
		break;
	case ID_EDIT_DELETE:
		onDelete();
		break;
	case ID_WINDOW_TIDE:
		SendMessage(hWndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0L);
		break;
	case ID_WINDOW_CASCADE:
		SendMessage(hWndClient, WM_MDICASCADE, 0, 0);
		break;
	case ID_WINDOW_CLOSEALL:
		EnumChildWindows(hWndClient, (WNDENUMPROC)MDICloseWndProc, 0L);
		break;
	case ID_DRAW_RECTANGLE:
		OnCheckItem(hWnd, 3);
		break;
	case ID_DRAW_ELLIPSE:
		OnCheckItem(hWnd, 4);
		break;
	case ID_DRAW_LINE:
		OnCheckItem(hWnd, 2);
		break;
	case ID_DRAW_SELECTOBJECT:
		OnCheckItem(hWnd, 6);
		break;
	case ID_DRAW_TEXT:
		OnCheckItem(hWnd, 5);
		break;
	case ID_DRAW_COLOR:
		onColorDlg(hWnd);
		break;
	case ID_DRAW_FONT:
		onFontDlg();
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	}
}


// Child Window
void initChildWindow(HWND hWnd)
{
	if (FlagInit == 1)
	{
		FlagInit = 0;
		HMENU hMenu = GetMenu(hWndFrame);
		EnableMenuItem(hMenu, 1, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, 2, MF_ENABLED | MF_BYPOSITION);
		hMenu = GetSubMenu(GetMenu(hWndFrame), 2);
		for (int i = 0; i < 7; i++)
			EnableMenuItem(hMenu, i, MF_ENABLED | MF_BYPOSITION);
		OnCheckItem(hWndFrame, 2);
		GetClientRect(hWnd, &ChildRec);
		DrawMenuBar(hWndFrame);
		return;
	}
	CHILD_WND_DATA *wndData = new CHILD_WND_DATA;
	wndData->hWnd = hWnd;
	wndData->Color = RGB(0, 0, 0);
	wndData->Click_EditBox = wndData->Clicked = NONE;
	wndData->IndexSelect = -1;
	wcscpy_s(wndData->szFile, L"");
	HFONT tmp = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(tmp, sizeof(LOGFONT), (LPSTR)&wndData->Font);

	SetLastError(0);
	if (SetWindowLongPtr(hWnd, 0, (LONG_PTR)wndData) != 0)
		MessageBox(hWnd, L"Can not set child data!", L"ERROR", MB_OK);
}

void onNewWnd(HWND hWnd, CHILD_WND_DATA *OpenData)
{
	MDICREATESTRUCT mdiCreate;
	ZeroMemory(&mdiCreate, sizeof(MDICREATESTRUCT));
	mdiCreate.szClass = L"NEWWINDOW";
	WCHAR title[255];
	wsprintf(title, L"Noname-%d.drw", New++);
	mdiCreate.szTitle = title;
	mdiCreate.hOwner = hInst;
	mdiCreate.x = CW_USEDEFAULT;
	mdiCreate.y = CW_USEDEFAULT;
	mdiCreate.cx = CW_USEDEFAULT;
	mdiCreate.cy = CW_USEDEFAULT;
	mdiCreate.style = 0;
	mdiCreate.lParam = NULL;

	if (OpenData != NULL)
		FlagInit = 1;
	HWND a = (HWND)SendMessage(hWndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdiCreate);
	if (OpenData != NULL)
	{
		OpenData->hWnd = a;
		OpenData->Click_EditBox = OpenData->Clicked = NONE;
		OpenData->IndexSelect = -1;
		SetWindowLongPtr(a, 0, (LONG_PTR)OpenData);
		SetWindowText(a, OpenData->szFile);
	}
}

void onMDIActivate(HWND hWnd)
{
	CHILD_WND_DATA *wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);
	if (wndData == NULL) return;

	wndData->Click_EditBox = wndData->Clicked = NONE;
	HMENU hMenu = GetMenu(hWndFrame);
	EnableMenuItem(hMenu, 1, MF_ENABLED | MF_BYPOSITION);
	EnableMenuItem(hMenu, 2, MF_ENABLED | MF_BYPOSITION);
	hMenu = GetSubMenu(GetMenu(hWndFrame), 2);
	for (int i = 0; i < 7; i++)
		EnableMenuItem(hMenu, i, MF_ENABLED | MF_BYPOSITION);
	GetClientRect(hWnd, &ChildRec);
	DrawMenuBar(hWndFrame);
}

//Message Mouse
void OnMouseClick(HWND hWnd, LPARAM lParam, RECT &Mouse, int &x, int &y)
{
	CHILD_WND_DATA* wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);
	if (FlagOpen == 1) return;
	RECT rc;
	POINT ca, cb;
	GetClientRect(hWnd, &rc);
	ca.x = rc.left;
	ca.y = rc.top;
	cb.x = rc.right;
	cb.y = rc.bottom;
	ClientToScreen(hWnd, &ca);
	ClientToScreen(hWnd, &cb);
	SetRect(&rc, ca.x, ca.y, cb.x, cb.y);
	ClipCursor(&rc);


	Mouse.left = x = LOWORD(lParam);
	Mouse.top = y = HIWORD(lParam);
	Mouse.right = 0;
	Mouse.bottom = 0;
	switch (ChooseMode)
	{
	case MD_TEXT:
	{
		WCHAR tmp[255];
		if (wndData->Click_EditBox == MOUSEUP)
		{
			hdc = GetDC(hWnd);
			RECT rec;
			GetClientRect(wndData->editbox, &rec);
			int len = GetWindowText(wndData->editbox, (LPWSTR)tmp, 255);
			DestroyWindow(wndData->editbox);
			ObjText *Text = new ObjText(wndData->Font, Mouseprev, wndData->Color, tmp, len);
			Text->Draw(hdc);
			wndData->Ob.push_back(Text);
			wndData->Click_EditBox = wndData->Clicked = NONE;
			ReleaseDC(hWnd, hdc);
		}
		else if (wndData->Click_EditBox == NONE)
		{
			Mouseprev.left = x;
			Mouseprev.top = y;
			wndData->Clicked = MOUSECLICK;
		}
		wndData->IndexSelect = -1;
	}
	break;
	case MD_SELECT:
	{
		//if (wndData->Ob.empty()) return;
		int x1 = LOWORD(lParam);
		int y1 = HIWORD(lParam);
		int i;
		for (i = wndData->Ob.size() - 1; i >= 0; i--)
			if (wndData->Ob[i]->IsIn(x1, y1))
			{
				wndData->IndexSelect = i;
				break;
			}
		if (i == -1)
			wndData->IndexSelect = -1;
		if (wndData->IndexSelect != -1)
		{
			switch (wndData->Ob[wndData->IndexSelect]->GetType())
			{
			case 0:
				tmp = new ObjLine;
				tmp = wndData->Ob[wndData->IndexSelect];
				break;
			case 1:
				tmp = new ObjRectangle;
				tmp = wndData->Ob[wndData->IndexSelect];
				break;
			case 2:
				tmp = new ObjEllipse;
				tmp = wndData->Ob[wndData->IndexSelect];
				break;
			case 3:
				tmp = new ObjText;
				tmp = wndData->Ob[wndData->IndexSelect];
				break;
			}
		}
	}
	break;
	default:
		wndData->IndexSelect = -1;
		break;
	}

}

void OnMouseMove(HWND hWnd, LPARAM lParam, WPARAM wParam, RECT &Mouse, int &x, int &y)
{
	if (!(wParam & WM_LBUTTONDOWN))
	{
		return;
	}
	CHILD_WND_DATA* wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);

	hdc = GetDC(hWnd);
	memDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, ChildRec.right, ChildRec.bottom);
	SelectObject(memDC, hBitmap);
	FillRect(memDC, &ChildRec, (HBRUSH)GetStockObject(WHITE_BRUSH));

	SetROP2(memDC, R2_MASKPEN);

	HPEN hPen = CreatePen(PS_SOLID, 1, wndData->Color);
	SelectObject(memDC, hPen);
	switch (ChooseMode)
	{
	case MD_LINE:
	{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		MoveToEx(memDC, Mouse.left, Mouse.top, NULL);
		LineTo(memDC, x, y);
	}
	break;
	case MD_RECTANGLE:
	{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		Rectangle(memDC, Mouse.left, Mouse.top, x, y);
	}
	break;
	case MD_ELLIPSE:
	{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		Ellipse(memDC, Mouse.left, Mouse.top, x, y);
	}
	break;
	case MD_TEXT:
	{
		if (wndData->Clicked == MOUSECLICK)
		{
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			Rectangle(memDC, Mouse.left, Mouse.top, x, y);
			wndData->Click_EditBox = MOUSEMOVE;
		}
	}
	break;
	case MD_SELECT:
	{
		int x1 = x, y1 = y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (wndData->IndexSelect > -1)
		{
			wndData->Ob[wndData->IndexSelect]->Move(x - x1, y - y1);
		}
	}
	}
	DrawAgain(hWnd);
	BitBlt(hdc, 0, 0, ChildRec.right, ChildRec.bottom, memDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
	DeleteObject(hBitmap);
	ReleaseDC(hWnd, hdc);
}

void OnMouseUp(HWND hWnd, LPARAM lParam, RECT &Mouse)
{
	if (FlagOpen == 1) return;
	hdc = GetDC(hWnd);
	memDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, ChildRec.right, ChildRec.bottom);
	SelectObject(memDC, hBitmap);
	FillRect(memDC, &ChildRec, (HBRUSH)GetStockObject(WHITE_BRUSH));

	Mouse.right = LOWORD(lParam);
	Mouse.bottom = HIWORD(lParam);

	CHILD_WND_DATA* wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);
	ClipCursor(NULL);
	SetROP2(hdc, R2_MASKPEN);
	Object *ob;
	switch (ChooseMode)
	{
	case MD_LINE:
	{
		ob = new ObjLine(Mouse, wndData->Color);
		ob->Draw(memDC);
		wndData->Ob.push_back(ob);
	}
	break;
	case MD_RECTANGLE:
	{
		ObjRectangle *Rec = new ObjRectangle(Mouse, wndData->Color);
		Rec->Draw(memDC);
		wndData->Ob.push_back(Rec);
	}
	break;
	case MD_ELLIPSE:
	{
		ObjEllipse *Ell = new ObjEllipse(Mouse, wndData->Color);
		Ell->Draw(memDC);
		wndData->Ob.push_back(Ell);
	}
	break;
	case MD_TEXT:
	{
		if (wndData->Click_EditBox == MOUSEMOVE)
		{
			wndData->editbox = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL,
				Mouse.left, Mouse.top, Mouse.right - Mouse.left, Mouse.bottom - Mouse.top, hWnd, nullptr, hInst, nullptr);
			SetFocus(wndData->editbox);
			ShowWindow(wndData->editbox, SW_SHOW);
			wndData->Click_EditBox = MOUSEUP;
			Mouseprev.right = Mouse.right;
			Mouseprev.bottom = Mouse.bottom;
		}
	}
	break;
	case MD_SELECT:
	{
		if (wndData->IndexSelect > -1)
		{
			wndData->Ob[wndData->IndexSelect] = tmp;
			wndData->Ob[wndData->IndexSelect]->Select(memDC);
		}
	}
	break;
	}
	DrawAgain(hWnd);
	BitBlt(hdc, 0, 0, ChildRec.right, ChildRec.bottom, memDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
	ReleaseDC(hWnd, hdc);
}

void DrawAgain(HWND hWnd)
{
	CHILD_WND_DATA* wndData = (CHILD_WND_DATA*)GetWindowLongPtr(hWnd, 0);
	SetROP2(memDC, R2_MASKPEN);
	for (int i = 0; i < wndData->Ob.size(); i++)
	{
		wndData->Ob[i]->Draw(memDC);
	}
	if (wndData->IndexSelect > -1)
		wndData->Ob[wndData->IndexSelect]->Select(memDC);
}

//Common Dialog
void onColorDlg(HWND hWnd)
{
	static COLORREF acrCustClr[100]; // array of custom colors 

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWndFrame;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *StaticData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	if (StaticData != NULL)
	{
		cc.rgbResult = StaticData->Color;
	}
	if (ChooseColor(&cc) == TRUE)
	{
		StaticData->Color = cc.rgbResult;
		if (StaticData->IndexSelect > -1)
		{
			StaticData->Ob[StaticData->IndexSelect]->ChangeColor(cc.rgbResult);
			hdc = GetDC(a);
			memDC = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, ChildRec.right, ChildRec.bottom);
			SelectObject(memDC, hBitmap);
			FillRect(memDC, &ChildRec, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawAgain(a);
			BitBlt(hdc, 0, 0, ChildRec.right, ChildRec.bottom, memDC, 0, 0, SRCCOPY);
			DeleteDC(memDC);
			ReleaseDC(a, hdc);
		}
	}
}

void onFontDlg()
{
	CHOOSEFONT cf;            // common dialog box structure
	static LOGFONT lf;        // logical font structure
	static COLORREF rgbCurrent;
	// Initialize CHOOSEFONT
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hWndFrame;
	cf.rgbColors = rgbCurrent;
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *StaticData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);

	if (ChooseFont(&cf) == TRUE)
	{
		StaticData->Font = lf;
		if (StaticData->IndexSelect > -1)
		{
			StaticData->Ob[StaticData->IndexSelect]->ChangeFont(lf);
			hdc = GetDC(a);
			memDC = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, ChildRec.right, ChildRec.bottom);
			SelectObject(memDC, hBitmap);
			FillRect(memDC, &ChildRec, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawAgain(a);
			BitBlt(hdc, 0, 0, ChildRec.right, ChildRec.bottom, memDC, 0, 0, SRCCOPY);
			DeleteDC(memDC);
			ReleaseDC(a, hdc);
		}
	}
}

void onOpenDlg(HWND hWnd, CHILD_WND_DATA *&OpenData)
{
	OPENFILENAME ofn;       // common dialog box structure
	FlagOpen = 1;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndFrame;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Bitmap\0*.BMP\0Draw\0*.drw\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		WCHAR tmp[255];
		if (!SeparateFileName(szFile, tmp))
			MessageBox(hWndFrame, L"It is not a *.drw file", L"ERROR", MB_OK);
		else
		{
			fstream iof;
			iof.open(szFile, ios::in | ios::binary);
			SeparateFileName(szFile, tmp);
			ReadFileWnd(iof, OpenData);
			wcscpy_s(OpenData->szFile, tmp);
			iof.close();
			onNewWnd(hWndFrame, OpenData);
			FlagOpen = 0;

		}
	}
}

void onSaveAsDlg()
{
	OPENFILENAME ofn;       // common dialog box structure

	WCHAR szFile[260];       // buffer for file name

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndFrame;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0Bitmap\0*.BMP\0Draw\0*.drw\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *SaveData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	// Display the Open dialog box. 
	if (SaveData == NULL) return;
	if (wcscmp(SaveData->szFile, L"") != 0)
	{
		WriteFileWnd(SaveData->szFile, SaveData);
		MessageBox(SaveData->hWnd, L"File is saved successfully!", L"Notice", MB_OK);
		return;
	}

	if (GetSaveFileName(&ofn) == TRUE)
	{
		SeparateFileName(szFile, SaveFileName);
		if (wcswcs(SaveFileName, L".drw") == NULL)
		{
			wcscat_s(SaveFileName, L".drw");
			wcscat_s(szFile, L".drw");
		}
		if (WriteFileWnd(szFile, SaveData)) {

			wcscpy_s(SaveData->szFile, szFile);
			SetWindowText(SaveData->hWnd, SaveFileName);
			MessageBox(SaveData->hWnd, L"File is saved successfully!", L"Notice", MB_OK);
		}
		else {
			MessageBox(SaveData->hWnd, L"File is not saved successfully!", L"ERROR", MB_OK);
			FlagOpen = 0;
		}
	}


}

//Edit command 
void onCut()
{
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *StaticData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	if (StaticData->IndexSelect == -1) return;
	//Copy to clipboard
	int index = StaticData->IndexSelect;
	int type = StaticData->Ob[index]->GetType();
	switch (type)
	{
	case 0:
	case 1:
	case 2:
	{
		int IDLineRectEll = RegisterClipboardFormat(L"LineRectEll");
		HGLOBAL hgMem = GlobalAlloc(GHND, sizeof(ObjectLineRectEll));
		ObjectLineRectEll *tmp = (ObjectLineRectEll*)GlobalLock(hgMem);
		tmp->Color = StaticData->Ob[index]->GetColor();
		tmp->Size = StaticData->Ob[index]->GetSize();
		tmp->Size.right -= tmp->Size.left - 100; tmp->Size.bottom -= tmp->Size.top - 100;
		tmp->Size.left = 100; tmp->Size.top = 100;
		tmp->Type = StaticData->Ob[index]->GetType();
		GlobalUnlock(hgMem);
		if (OpenClipboard(hWndFrame))
		{
			EmptyClipboard();
			SetClipboardData(IDLineRectEll, hgMem);
			CloseClipboard();
		}
	}
	break;
	case 3:
	{
		int IDText = RegisterClipboardFormat(L"Text");
		HGLOBAL hgMem = GlobalAlloc(GHND, sizeof(ObjectText));
		ObjectText *tmp = (ObjectText*)GlobalLock(hgMem);
		tmp->Color = StaticData->Ob[index]->GetColor();
		tmp->Size = StaticData->Ob[index]->GetSize();
		tmp->Size.right -= tmp->Size.left - 100; tmp->Size.bottom -= tmp->Size.top - 100;
		tmp->Size.left = 100; tmp->Size.top = 100;
		StaticData->Ob[index]->GetFont(tmp->Font);
		tmp->len = StaticData->Ob[index]->Getlength();
		StaticData->Ob[index]->GetText(tmp->_Text);
		GlobalUnlock(hgMem);
		if (OpenClipboard(hWndFrame))
		{
			EmptyClipboard();
			SetClipboardData(IDText, hgMem);
			CloseClipboard();
		}
	}
	break;
	}
	// Delete Object

	StaticData->Ob.erase(StaticData->Ob.begin() + index);
	StaticData->IndexSelect = -1;
	InvalidateRect(a, NULL, TRUE);

}

void onCopy()
{
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *StaticData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	if (StaticData->IndexSelect == -1) return;
	int index = StaticData->IndexSelect;
	int type = StaticData->Ob[index]->GetType();
	switch (type)
	{
	case 0:
	case 1:
	case 2:
	{
		int IDLineRectEll = RegisterClipboardFormat(L"LineRectEll");
		HGLOBAL hgMem = GlobalAlloc(GHND, sizeof(ObjectLineRectEll));
		ObjectLineRectEll *tmp = (ObjectLineRectEll*)GlobalLock(hgMem);
		tmp->Color = StaticData->Ob[index]->GetColor();
		tmp->Size = StaticData->Ob[index]->GetSize();
		tmp->Size.right -= tmp->Size.left - 100; tmp->Size.bottom -= tmp->Size.top - 100;
		tmp->Size.left = 100; tmp->Size.top = 100;
		tmp->Type = StaticData->Ob[index]->GetType();
		GlobalUnlock(hgMem);
		if (OpenClipboard(hWndFrame))
		{
			EmptyClipboard();
			SetClipboardData(IDLineRectEll, hgMem);
			CloseClipboard();
		}
	}
	break;
	case 3:
	{
		int IDText = RegisterClipboardFormat(L"Text");
		HGLOBAL hgMem = GlobalAlloc(GHND, sizeof(ObjectText));
		ObjectText *tmp = (ObjectText*)GlobalLock(hgMem);
		tmp->Color = StaticData->Ob[index]->GetColor();
		tmp->Size = StaticData->Ob[index]->GetSize();
		tmp->Size.right -= tmp->Size.left - 100; tmp->Size.bottom -= tmp->Size.top - 100;
		tmp->Size.left = 100; tmp->Size.top = 100;
		StaticData->Ob[index]->GetFont(tmp->Font);
		tmp->len = StaticData->Ob[index]->Getlength();
		StaticData->Ob[index]->GetText(tmp->_Text);
		GlobalUnlock(hgMem);
		if (OpenClipboard(hWndFrame))
		{
			EmptyClipboard();
			SetClipboardData(IDText, hgMem);
			CloseClipboard();
		}
	}
	break;
	}
}

void onPaste()
{
	char* Text = NULL;
	size_t len = 0;
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *StaticData = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	if (OpenClipboard(hWndFrame))
	{
		// OBJECT LINE RECTANGLE ELLPISE
		int IDLineRectEll = RegisterClipboardFormat(L"LineRectEll");
		HGLOBAL hgMem = GetClipboardData(IDLineRectEll);
		if (hgMem)
		{
			ObjectLineRectEll *tmp = (ObjectLineRectEll*)GlobalLock(hgMem);
			ObjectLineRectEll e;
			CopyMemory(&e, tmp, sizeof(ObjectLineRectEll));
			GlobalUnlock(hgMem);
			Object *ob;
			switch (e.Type)
			{
			case 0:
				ob = new ObjLine(e.Size, e.Color);
				StaticData->Ob.push_back(ob);
				break;
			case 1:
				ob = new ObjRectangle(e.Size, e.Color);
				StaticData->Ob.push_back(ob);
				break;
			case 2:
				ob = new ObjEllipse(e.Size, e.Color);
				StaticData->Ob.push_back(ob);
				break;
			}
			CloseClipboard();
			InvalidateRect(a, NULL, TRUE);
		}
		else
		{
			//OBJECT TEXT
			int IDText = RegisterClipboardFormat(L"Text");
			hgMem = GetClipboardData(IDText);
			if (hgMem)
			{
				ObjectText *tmp = (ObjectText*)GlobalLock(hgMem);
				ObjectText e;
				CopyMemory(&e, tmp, sizeof(ObjectText));
				GlobalUnlock(hgMem);
				Object *obText = new ObjText(e.Font, e.Size, e.Color, e._Text, e.len);
				StaticData->Ob.push_back(obText);
				CloseClipboard();
				InvalidateRect(a, NULL, TRUE);
			}
			else
			{
				// CF_TEXT
				HANDLE handle = (HANDLE)GetClipboardData(CF_TEXT);
				if (handle != NULL)
				{
					char* p = (char*)GlobalLock(handle);
					len = strlen(p) + 1;
					Text = new char[len];
					strcpy_s(Text, len, p);
					GlobalUnlock(handle);
					WCHAR tmp[256];
					size_t *pOfChar = new size_t;
					*pOfChar = 256;
					mbstowcs_s(pOfChar, tmp, Text, len);
					Object *obCFText = new ObjText(tmp, StaticData->Font, StaticData->Color);
					StaticData->Ob.push_back(obCFText);
					CloseClipboard();
					InvalidateRect(a, NULL, TRUE);
				}
			}
		}
	}
}

void onDelete()
{
	HWND a = GetWindow(hWndClient, GW_CHILD);
	CHILD_WND_DATA *tmp = (CHILD_WND_DATA*)GetWindowLongPtr(a, 0);
	if (tmp->IndexSelect > -1)
	{
		int i = tmp->IndexSelect;
		tmp->Ob.erase(tmp->Ob.begin() + i);
		tmp->IndexSelect = -1;
		InvalidateRect(a, NULL, TRUE);
	}
}

//Toolbar
void doCreate_ToolBar()
{
	// loading Common Control DLL
	InitCommonControls();

	TBBUTTON tbButtons[] =
	{
		{ STD_FILENEW,	ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_FILEOPEN,	ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_FILESAVE,	ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_CUT,		ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_COPY,		ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_PASTE,	ID_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_DELETE,	ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
	};

	// create a toolbar
	hWndToolbar = CreateToolbarEx(hWndFrame,
		WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS,
		ID_TOOLBAR,
		sizeof(tbButtons) / sizeof(TBBUTTON),
		HINST_COMMCTRL,
		0,
		tbButtons,
		sizeof(tbButtons) / sizeof(TBBUTTON),
		BUTTON_WIDTH,
		BUTTON_HEIGHT,
		IMAGE_WIDTH,
		IMAGE_HEIGHT,
		sizeof(TBBUTTON));
}

void doToolBar_AddUserButton()
{
	// define new buttons
	TBBUTTON tbButtons[] =
	{
		{ 0, 0					 ,	TBSTATE_ENABLED, TBSTYLE_SEP   , 0, 0 },
		{ 0, ID_DRAW_LINE		 ,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 1, ID_DRAW_RECTANGLE	 ,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 2, ID_DRAW_ELLIPSE	 ,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 3, ID_DRAW_TEXT		 ,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 4, ID_DRAW_SELECTOBJECT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
	};

	// structure contains the bitmap of user defined buttons. It contains 2 icons
	TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };

	// Add bitmap to Image-list of ToolBar
	int idx = SendMessage(hWndToolbar, TB_ADDBITMAP, (WPARAM) sizeof(tbBitmap) / sizeof(TBADDBITMAP),
		(LPARAM)(LPTBADDBITMAP)&tbBitmap);

	// identify the bitmap index of each button
	tbButtons[1].iBitmap += idx;
	tbButtons[2].iBitmap += idx;
	tbButtons[3].iBitmap += idx;
	tbButtons[4].iBitmap += idx;
	tbButtons[5].iBitmap += idx;

	// add buttons to toolbar
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM) sizeof(tbButtons) / sizeof(TBBUTTON),
		(LPARAM)(LPTBBUTTON)&tbButtons);
}

