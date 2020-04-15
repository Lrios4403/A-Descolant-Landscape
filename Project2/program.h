#ifndef PCPROGRAM_H
#define PCPROGRAM_H

#include <iostream>
#include <functional>
#include <string.h>
#include <vector>

#include "olcConsoleGameEngine.h"
#include "object.h"
#include "tdmath.h"
#include "debug.h"

struct PcButton
{
	int x, y, iwidth, iheight;
	olcSprite* spriteInside;
	std::function<int()> function;
};

class PcProgram
{
public:
	bool onTaskbar = false,  onDisplay = false, isNeeded = false;
	POINT pDesktop = { 0,0 };

	int iTaskbarIndex = 0;
	int iDesktopIndex = 0;

	olcSprite* spriteIcon = new olcSprite(L"resources\\pcdefaulticon.png.spr");
	olcSprite* windowSprite;
	vec2d windowPosition;
	vec2d desktopPosition;

	//Basic class calls for programs
	virtual int Initalize() = 0;
	virtual int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) = 0;
	virtual int Display() = 0;
	virtual int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) = 0;
	virtual int MouseUpdate(int x, int y) = 0;

	//Protected class calls for program
protected:
	int AddSpriteToWindow(olcSprite* sprite, int px, int py)
	{
		for (int w = 0; w < sprite->nWidth; w++)
		{
			for (int h = 0; h < sprite->nHeight; h++)
			{
				//Get the x & y position to set the window
				int x = px + w;
				int y = py + h;

				//Check the bounds of the clip
				if (x < 0 || y < 0) continue;
				if (x > windowSprite->nWidth || y > windowSprite->nHeight) continue;

				if (sprite->GetGlyph(w, h) == 32) continue;

				windowSprite->SetColour(x, y, sprite->GetColour(w, h));
				windowSprite->SetGlyph(x, y, sprite->GetGlyph(w, h));
			}
		}

		return 0;
	}
	int CenterWindow()
	{
		windowPosition.u = 150 - (windowSprite->nWidth / 2);
		windowPosition.v = 50 - (windowSprite->nHeight / 2);

		return 0;
	}
	inline bool IfMouseClick(int mouseX, int mouseY, olcConsoleGameEngine* olc)
	{
		//Test if the mouse is inside the window
		if (mouseX >= windowPosition.u && mouseX <= windowPosition.u + windowSprite->nWidth)
			if (mouseY >= windowPosition.v && mouseY <= windowPosition.v + windowSprite->nHeight)
				if (olc->GetMouse(0).bPressed)
					return true;
		return false;
	}
};

#endif