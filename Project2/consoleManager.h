#ifndef CONSOLEMANANGER_H
#define CONSOLEMANANGER_H

#include "olcConsoleGameEngine.h"

#include "anticheat.h"
#include "sound.h"
#include "steam.h"

#include "debug.h"
#include "macro.h"

#include <windows.h>

//Needed for easier reading
void* pConsoleManager = nullptr;
#define ConsolePointerMacro ((ConsoleManager*)pConsoleManager)

DWORD defColorPalette[16] =
{
		789516,
		14300928,
		958739,
		14521914,
		2035653,
		9967496,
		40129,
		13421772,
		7763574,
		16742459,
		837142,
		14079585,
		5654759,
		10354868,
		10875385,
		15921906
};

class ConsoleManager
{
private:
	CONSOLE_SCREEN_BUFFER_INFOEX	infoCurrent;
	CONSOLE_FONT_INFOEX				infoFont;

public:

	float fFps = 0;
	bool bCaptureMouse = true;

	int Initalize(olcConsoleGameEngine* olc)
	{
		odprintf("Initalizing ConsoleManager[%p]...", this);
		debugTabIndex++;

		//Handle the console colors
		infoCurrent.cbSize = sizeof(infoCurrent);
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfoEx(hConsole, &infoCurrent);
		for (int i = 0; i < 16; i++)infoCurrent.ColorTable[i] = defColorPalette[i];
		SetConsoleScreenBufferInfoEx(hConsole, &infoCurrent);

		//Handle the console fonts
		GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), true, &infoFont);
		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), true, &infoFont);

		pConsoleManager = this;

		debugTabIndex--;
		return 0;
	}

	inline _CONSOLE_CURSOR_INFO GetPalette()
	{
		_CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		return info;
	}

	inline void SetCursorInfo(PCONSOLE_CURSOR_INFO info)
	{
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), info);
	}

	inline void SetCursorVisible(bool b)
	{
		_CONSOLE_CURSOR_INFO info = GetPalette();
		info.bVisible = b;
		SetCursorInfo(&info);
	}

	int ShowConsole(bool bShow)
	{
		if (bShow)
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		else
			ShowWindow(GetConsoleWindow(), SW_HIDE);

		return 0;
	}

	bool CenterWindow()
	{
		HWND ConsoleWindow;
		ConsoleWindow = GetForegroundWindow();

		//Getting the desktop hadle and rectangule
		HWND   hwndScreen;
		RECT   rectScreen;
		hwndScreen = GetDesktopWindow();
		GetWindowRect(hwndScreen, &rectScreen);

		RECT rConsole;
		GetWindowRect(GetConsoleWindow(), &rConsole);
		int Width = rConsole.left = rConsole.right;
		int Height = rConsole.bottom - rConsole.top;

		//caculate the window console to center of the screen	
		int ConsolePosX;
		int ConsolePosY;
		ConsolePosX = ((rectScreen.right - rectScreen.left) / 2 - Width / 2);
		ConsolePosY = ((rectScreen.bottom - rectScreen.top) / 2 - Height / 2);
		SetWindowPos(GetConsoleWindow(), NULL, ConsolePosX, ConsolePosY, Width, Height, SWP_SHOWWINDOW || SWP_NOSIZE);

		SwitchToThisWindow(GetConsoleWindow(), true);

		int width = rectScreen.right - rectScreen.left;
		int height = rectScreen.bottom - rectScreen.top;
		POINT cursorPos;
		SetCursorPos((width / 2) + rectScreen.left, (height / 2) + rectScreen.top);

		return true;
	}

	POINT GetTerminalSize()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns, rows;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

		return { columns,rows};

	}

	int Destroy()
	{
		odprintf("ConsoleManager[%p].Destroy()...", this);
		debugTabIndex++;

		debugTabIndex--;
		return 0;
	}

};

#endif