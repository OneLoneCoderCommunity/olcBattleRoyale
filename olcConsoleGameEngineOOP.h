/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9

Disclaimer
~~~~~~~~~~
I don't care what you use this for. It's intended to be educational, and perhaps
to the oddly minded - a little bit of fun. Please hack this, change it and use it
in any way you see fit. BUT, you acknowledge that I am not responsible for anything
bad that happens as a result of your actions. However, if good stuff happens, I
would appreciate a shout out, or at least give the blog some publicity for me.
Cheers!

Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.

Author
~~~~~~
Twitter: @javidx9 http://twitter.com/javidx9
Blog: http://www.onelonecoder.com
YouTube: http://www.youtube.com/javidx9

Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o

Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
YouTube: 	wowLinh, JavaJack59, idkwid, kingtatgi

Last Updated: 04/02/2018

Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.

int main()
{
// Use olcConsoleGameEngine derived app
OneLoneCoder_Example game;

// Create a console with resolution 160x100 characters
// Each character occupies 8x8 pixels
game.ConstructConsole(160, 100, 8, 8);

// Start the engine!
game.Start();

return 0;
}

Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.

The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.

There may be bugs!

See my other videos for examples!
http://www.youtube.com/javidx9

Lots of programs to try:
http://www.github.com/OneLoneCoder/videos

Chat on the Discord server:
https://discord.gg/WhwHUMV

Be bored by Twitch:
http://www.twitch.tv/javidx9



bool IsWin8OrLater()
{
DWORD version = GetVersion();
DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

return (major > 6) || ((major == 6) && (minor >= 2) && (minor < 4));
}

*/

#pragma once

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <sstream>
#include <streambuf>
using namespace std;

#include <windows.h>


enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class olcSprite
{
public:
	olcSprite()
	{

	}

	olcSprite(int w, int h)
	{
		Create(w, h);
	}

	olcSprite(wstring sFile)
	{
		if (!Load(sFile))
			Create(8, 8);
	}

	int nWidth = 0;
	int nHeight = 0;

private:
	wchar_t *m_Glyphs = nullptr;
	short *m_Colours = nullptr;

	void Create(int w, int h)
	{
		nWidth = w;
		nHeight = h;
		m_Glyphs = new wchar_t[w*h];
		m_Colours = new short[w*h];
		for (int i = 0; i < w*h; i++)
		{
			m_Glyphs[i] = L' ';
			m_Colours[i] = FG_BLACK;
		}
	}

public:
	void SetGlyph(int x, int y, wchar_t c)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Glyphs[y * nWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Colours[y * nWidth + x] = c;
	}

	wchar_t GetGlyph(int x, int y)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return L' ';
		else
			return m_Glyphs[y * nWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[y * nWidth + x];
	}

	wchar_t SampleGlyph(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx <0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return L' ';
		else
			return m_Glyphs[sy * nWidth + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx <0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[sy * nWidth + sx];
	}

	bool Save(wstring sFile)
	{
		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite(&nWidth, sizeof(int), 1, f);
		fwrite(&nHeight, sizeof(int), 1, f);
		fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
		fwrite(m_Glyphs, sizeof(wchar_t), nWidth * nHeight, f);

		fclose(f);

		return true;
	}

	bool Load(wstring sFile)
	{
		delete[] m_Glyphs;
		delete[] m_Colours;
		nWidth = 0;
		nHeight = 0;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		fread(&nWidth, sizeof(int), 1, f);
		fread(&nHeight, sizeof(int), 1, f);

		Create(nWidth, nHeight);

		fread(m_Colours, sizeof(short), nWidth * nHeight, f);
		fread(m_Glyphs, sizeof(wchar_t), nWidth * nHeight, f);

		fclose(f);
		return true;
	}

	bool LoadFromResource(unsigned int id)
	{
		HRSRC res = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
		if (!res) return false;

		HGLOBAL res_handle = LoadResource(NULL, res);
		if (!res_handle) return false;

		char* res_data = (char*)LockResource(res_handle);
		DWORD res_size = SizeofResource(NULL, res);


		


		istringstream iss(res_data);
		istream *is = &iss;
		
		delete[] m_Glyphs;
		delete[] m_Colours;
		nWidth = 0;
		nHeight = 0;

		is->read((char*)&nWidth, sizeof(int) * 1);
		is->read((char*)&nHeight, sizeof(int) * 1);
		
		Create(nWidth, nHeight);
		
		iss.read((char*)m_Colours, sizeof(short) * nWidth * nHeight);		
		iss.read((char*)m_Glyphs, sizeof(wchar_t) * nWidth * nHeight);


		return true;
	}

};


class olcConsoleGameEngineOOP
{
public:
	olcConsoleGameEngineOOP();
	~olcConsoleGameEngineOOP();

public:
	int ConstructConsole(int width, int height, int fontw, int fonth);
	void Start();
	
public:
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F);
	void Fill(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F);
	void DrawString(int x, int y, wstring c, short col = 0x000F);
	void DrawStringAlpha(int x, int y, wstring c, short col = 0x000F);
	void Clip(int &x, int &y);
	void DrawLine(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F);
	void DrawCircle(int xc, int yc, int r, wchar_t c = 0x2588, short col = 0x000F);
	void FillCircle(int xc, int yc, int r, wchar_t c = 0x2588, short col = 0x000F);
	void DrawSprite(int x, int y, olcSprite *sprite);
	void DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h);
	void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE, wchar_t c = PIXEL_SOLID);
	int ScreenWidth();
	int ScreenHeight();

private:
	void GameThread();

protected:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;

	// Optional for clean up 
	virtual bool OnUserDestroy();


	int Error(const wchar_t *msg);
	static BOOL CloseHandler(DWORD evt);

protected:


	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

public:
	sKeyState GetKey(int nKeyID) { return m_keys[nKeyID]; }
	int GetMouseX() { return m_mousePosX; }
	int GetMouseY() { return m_mousePosY; }
	sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }


protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	CHAR_INFO *m_bufScreen;
	wstring m_sAppName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;
	static atomic<bool> m_bAtomActive;
	static condition_variable m_cvGameFinished;
	static mutex m_muxGame;
};

