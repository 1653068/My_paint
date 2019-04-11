#include "stdafx.h"
#include "Object.h"

void Swap(LONG &a, LONG &b)
{
	LONG tmp = a;
	a = b;
	b = tmp;
}

//Object
Object::Object()
{
	Size.left = 0;
	Size.right = 0;
	Size.bottom = 0;
	Size.top = 0;
	Color = RGB(0, 0, 0);
}

Object::Object(RECT rec)
{
	Size = rec;
	if (Size.left > Size.right)
		Swap(Size.left, Size.right);
	if (Size.top > Size.bottom)
		Swap(Size.top, Size.bottom);
}

Object::~Object()
{
}

// Rectangle
ObjRectangle::ObjRectangle() : Object()
{
	Type = 1;
}

ObjRectangle::ObjRectangle(RECT ls, COLORREF cl) : Object(ls)
{
	Color = cl;
	Type = 1;
}

ObjRectangle::~ObjRectangle() 
{
	DeleteObject(hPen);
}

void ObjRectangle::Draw(HDC hdc)
{
	hPen = CreatePen(PS_SOLID, 1, Color);
	SelectObject(hdc, hPen);
	Rectangle(hdc, Size.left, Size.top, Size.right, Size.bottom);
	DeleteObject(hPen);
}

void ObjRectangle::Write(fstream &file)
{
	file.write((char*)&Type, sizeof(int));
	file.write((char*)&Size, sizeof(RECT));
	file.write((char*)&Color, sizeof(COLORREF));
}

void ObjRectangle::Read(fstream &file)
{
	file.read((char*)&Size, sizeof(RECT));
	file.read((char*)&Color, sizeof(COLORREF));
}

bool ObjRectangle::IsIn(int x, int y)
{
	if (x <= Size.right && x >= Size.left && y <= Size.bottom && y >= Size.top)
		return true;	
	return false;
}

int ObjRectangle::GetType()
{
	return Type;
}

//Ellipse
ObjEllipse::ObjEllipse() : Object()
{
	Type = 2;
}

ObjEllipse::ObjEllipse(RECT ls, COLORREF cl) : Object(ls)
{
	Color = cl;
	Type = 2;
}

ObjEllipse::~ObjEllipse()
{
	DeleteObject(hPen);
}

void ObjEllipse::Draw(HDC hdc)
{
	hPen = CreatePen(PS_SOLID, 1, Color);
	SelectObject(hdc, hPen);
	Ellipse(hdc, Size.left, Size.top, Size.right, Size.bottom);
	DeleteObject(hPen);
}

void ObjEllipse::Write(fstream &file)
{
	file.write((char*)&Type, sizeof(int));
	file.write((char*)&Size, sizeof(RECT));
	file.write((char*)&Color, sizeof(COLORREF));
}

void ObjEllipse::Read(fstream &file)
{
	file.read((char*)&Size, sizeof(RECT));
	file.read((char*)&Color, sizeof(COLORREF));
}

bool ObjEllipse::IsIn(int x, int y)
{
	float a = (Size.right - Size.left) / 2, b = (Size.bottom - Size.top) / 2;
	float Ox = Size.left + a, Oy = Size.top + b;
	float X = Ox - x, Y = Oy - y;
	if ((X*X) / (a*a) + (Y*Y) / (b*b) <= 1) 
		return true;
	return false;
}

int ObjEllipse::GetType()
{
	return Type;
}

//Line
ObjLine::ObjLine() :Object()
{
	Type = 0;
}

ObjLine::ObjLine(RECT ls, COLORREF cl)
{
	Size = ls;
	Color = cl;
	Type = 0;
}

ObjLine::~ObjLine()
{
	DeleteObject(hPen);
}

void ObjLine::Draw(HDC hdc)
{
	hPen = CreatePen(PS_SOLID, 1, Color);
	SelectObject(hdc, hPen);
	MoveToEx(hdc, Size.left, Size.top, NULL);
	LineTo(hdc, Size.right, Size.bottom);
	DeleteObject(hPen);
}

void ObjLine::Write(fstream &file)
{
	file.write((char*) &Type, sizeof(int));
	file.write((char*)&Size, sizeof(RECT));
	file.write((char*)&Color, sizeof(COLORREF));
}

void ObjLine::Read(fstream &file)
{
	file.read((char*)&Size, sizeof(RECT));
	file.read((char*)&Color, sizeof(COLORREF));
}

bool ObjLine::IsIn(int x, int y)
{
	if ((Size.left <= Size.right && Size.top <= Size.bottom && (x < Size.left || x > Size.right || y < Size.top || y > Size.bottom)) ||
		(Size.left >= Size.right && Size.top <= Size.bottom && (x > Size.left || x < Size.right || y < Size.top || y > Size.bottom)) ||
		(Size.left >= Size.right && Size.top >= Size.bottom && (x > Size.left || x < Size.right || y > Size.top || y < Size.bottom)) ||
		(Size.left <= Size.right && Size.top >= Size.bottom && (x < Size.left || x > Size.right || y > Size.top || y < Size.bottom)))
		return false;

	float a = (float)(Size.bottom - Size.top) / (Size.right - Size.left);
	float b = Size.top - Size.left*a;
	if (a*x + b - 3 <= y && y <= a * x + b + 3) {
		return true;
	}

	return false;
}

void ObjLine::Select(HDC hdc)
{
	SetROP2(hdc, R2_MASKPEN);
	HPEN hSelect = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
	SelectObject(hdc, hSelect);
	if (Size.left <= Size.right && Size.top <= Size.bottom)
		Rectangle(hdc, Size.left - 3, Size.top - 3, Size.right + 3, Size.bottom + 3);
	else if (Size.left >= Size.right && Size.top <= Size.bottom)
		Rectangle(hdc, Size.left + 3, Size.top - 3, Size.right - 3, Size.bottom + 3);
	else if (Size.left >= Size.right && Size.top >= Size.bottom)
		Rectangle(hdc, Size.left + 3, Size.top + 3, Size.right - 3, Size.bottom - 3);
	else
		Rectangle(hdc, Size.left - 3, Size.top + 3, Size.right + 3, Size.bottom - 3);
	DeleteObject(hSelect);
}

int ObjLine::GetType()
{
	return Type;
}

//Text
ObjText::ObjText() : Object()
{
	Type = 3;
}

ObjText::ObjText(LOGFONT font, RECT rec, COLORREF color, WCHAR str[255], int len) : Object(rec)
{
	this->Font = font;
	Color = color;
	wcscpy_s(_Text, str);
	length = len;
	Type = 3;
	hFont = CreateFontIndirect(&Font);
}

ObjText::~ObjText()
{
	DeleteObject(hPen);
	DeleteObject(hFont);
}

ObjText::ObjText(WCHAR a[256], LOGFONT lf, COLORREF color) : Object()
{
	Size.left = 0; Size.top = 0;
	Size.right = 400; Size.bottom = 100;
	wcscpy_s(_Text, a);
	length = wcslen(_Text);
	Type = 3;
	Font = lf;
	hFont = CreateFontIndirect(&Font);
	this->Color = color;
}

void ObjText::Draw(HDC hdc)
{
	SelectObject(hdc, hFont);
	SetTextColor(hdc, Color);
	RECT tmp;
	ZeroMemory(&tmp, sizeof(RECT));
	DrawText(hdc, _Text, length, &tmp, DT_CALCRECT);
	Size.right = Size.left + tmp.right; Size.bottom = Size.top + tmp.bottom;
	DrawText(hdc, _Text, length, &Size, DT_EDITCONTROL | DT_WORDBREAK );
}

void ObjText::Write(fstream &file)
{
	file.write((char*)&Type, sizeof(int));
	file.write((char*)&Size, sizeof(RECT));
	file.write((char*)&Color, sizeof(COLORREF));
	file.write((char*)&Font, sizeof(LOGFONT));
	file.write((char*)&length, sizeof(int));
	file.write((char*)_Text, length*sizeof(WCHAR));
}

void ObjText::Read(fstream &file)
{
	file.read((char*)&Size, sizeof(RECT));
	file.read((char*)&Color, sizeof(COLORREF));
	file.read((char*)&Font, sizeof(LOGFONT));
	file.read((char*)&length, sizeof(int));
	file.read((char*)_Text, length*sizeof(WCHAR));
	_Text[length] = '\0';
	HFONT tmp = CreateFontIndirect(&Font);
	hFont = tmp;
}

void ObjText::ChangeFont(LOGFONT NewFont)
{
	Font = NewFont;
	HFONT tmp = CreateFontIndirect(&Font);
	hFont = tmp;
}

bool ObjText::IsIn(int x, int y)
{
	if (x <= Size.right && x >= Size.left && y <= Size.bottom && y >= Size.top)
		return true;
	return false;
}

void ObjText::GetFont(LOGFONT &a)
{
	a = Font;
}

void ObjText::GetText(WCHAR tmp[256])
{
	wcscpy_s(tmp, 256, _Text);
}

int ObjText::GetType()
{
	return Type;
}

int ObjText::Getlength()
{
	return length;
}