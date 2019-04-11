#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
class Object
{
public:
	virtual void Draw(HDC hdc) {}
	virtual bool IsIn(int x, int y) { return false; }
	virtual void Select(HDC hdc)
	{
		SetROP2(hdc, R2_MASKPEN);
		HPEN hSelect = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
		SelectObject(hdc, hSelect);
		Rectangle(hdc, Size.left - 3, Size.top - 3, Size.right + 3, Size.bottom + 3);
		DeleteObject(hSelect);
	}
	virtual void Move(int x, int y) 
	{
		Size.left += x; Size.right += x;
		Size.top += y;  Size.bottom += y;
	}
	virtual void Read(fstream &file) {}
	virtual void Write(fstream &file) {}
	virtual int GetType()
	{
		return 0;
	}
	virtual void ChangeColor(COLORREF NewColor)
	{
		Color = NewColor;
	}
	virtual void ChangeFont(LOGFONT NewFont) {}
	virtual RECT GetSize()
	{
		return Size;
	}
	virtual COLORREF GetColor()
	{
		return Color;
	}
	virtual void GetFont(LOGFONT &a) {}
	virtual void GetText(WCHAR tmp[256]) {}
	virtual int Getlength() { return 0; }
	~Object();
	Object();
	Object(RECT rec);
protected:
	COLORREF Color;
	HPEN hPen;
	RECT Size;
	int Type;
};

class ObjRectangle : public Object
{
public:
	void Draw(HDC hdc);
	void Read(fstream &file);
	void Write(fstream &file);
	bool IsIn(int x, int y);
	int GetType();
	ObjRectangle();
	ObjRectangle(RECT ls, COLORREF cl);
	~ObjRectangle();
};

class ObjEllipse : public Object
{
public:
	void Draw(HDC hdc);
	void Read(fstream &file);
	void Write(fstream &file);
	bool IsIn(int x, int y);
	int GetType();
	ObjEllipse();
	ObjEllipse(RECT ls, COLORREF cl) ;
	~ObjEllipse();
};

class ObjLine : public Object
{
public:
	void Draw(HDC hdc);
	void Read(fstream &file);
	void Write(fstream &file);
	bool IsIn(int x, int y);
	void Select(HDC hdc);
	int GetType();
	ObjLine();
	ObjLine(RECT ls, COLORREF cl);
	~ObjLine();
};

class ObjText : public Object
{
public:
	void Draw(HDC hdc);
	void Read(fstream &file);
	void Write(fstream &file);
	void ChangeFont(LOGFONT NewFont);
	bool IsIn(int x, int y);
	int GetType();
	void GetFont(LOGFONT &a);
	void GetText(WCHAR tmp[256]);
	int Getlength();
	ObjText();
	ObjText(LOGFONT font, RECT rec, COLORREF color, WCHAR str[255],int len);
	ObjText(WCHAR a[256], LOGFONT lf, COLORREF color);
	~ObjText();
private:
	WCHAR _Text[255];
	LOGFONT Font;
	HFONT hFont;
	int length;
};

