#ifndef PCHELLOWORLD_H
#define PCHELLOWORLD_H

#include <iostream>
#include <string.h>
#include <vector>

#include "program.h"

class PcHelloWorld: public PcProgram
{
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;

	olcSprite* helloWorldSprite;

	int Initalize() override
	{
		windowSprite = new olcSprite(80, 25);										//Setup the default window to display

		helloWorldSprite = new olcSprite(L"resources\\helloworld.spr");

		for (int i = 0; i < 80; i++)												//Clear the window sprite
			for (int j = 0; j < 25; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_BLACK);

			}

		onTaskbar = true;

		CenterWindow();

		return 0;
	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;

			for (int i = 0; i < 80; i++)												//Clear the window sprite
				for (int j = 0; j < 25; j++)
				{
					windowSprite->SetGlyph(i, j, PIXEL_SOLID);

					if (j > 1)
						windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
					else
						windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

				}

			AddSpriteToWindow(helloWorldSprite, 0, 2);

			windowSprite->SetGlyph(78, 0, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(78, 0, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(78, 1, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(78, 1, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(77, 0, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(77, 0, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(77, 1, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(77, 1, BG_CYAN | FG_RED);
		}

		//Test if the mouse is inside the window
		if (mouseX >= windowPosition.u && mouseX <= windowPosition.u + windowSprite->nWidth)
		{
			if (mouseY >= windowPosition.v && mouseY <= windowPosition.v + windowSprite->nHeight)
			{
				//Check if the mouse 
				if (olc->GetMouse(0).bHeld)
				{
					//Test to see if we are dragging the mouse
					if (front)
					if (mouseX != lastMouseX || mouseY != lastMouseY)										//We are dragging
					{
						if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v+1)				//We are moving the window
						{
							isdraggingwindow = true;
							isNeeded = true;
						}
					}
					else																					//We are clicking
					{	
						if (mouseX == windowPosition.u + 77 || mouseX == windowPosition.u + 78)
						{
							if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
							{
								exitProgram = true;

								for (int i = 0; i < 80; i++)												//Clear the window sprite
								{
									for (int j = 0; j < 25; j++)
									{
										windowSprite->SetGlyph(i, j, PIXEL_HALF);
										windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
									}
								}
							}
						}
					}
				}
				if (olc->GetMouse(0).bReleased)
				{
					if (isdraggingwindow)
					{
						isdraggingwindow = false;
						isNeeded = false;
					}

					if (exitProgram == true)
					{
						onDisplay = false;
						exitProgram = false;
					}
				}

			}
		}

		if (isdraggingwindow)
		{
			windowPosition.u += mouseX - lastMouseX;
			windowPosition.v += mouseY - lastMouseY;
		}

		return 0;
	}

	int Display() override
	{

		return 0;
	}

	int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) override
	{
		if (onDisplay == false)
		{
			onDisplay = true;

			fBootedTime = fElapsedTotal;

			started = false;

			CenterWindow();
		}

		return 0;
	}

	int MouseUpdate(int x, int y) override
	{
		this->lastMouseX = mouseX;
		this->lastMouseY = mouseY;
		this->mouseX = x;
		this->mouseY = y;

		return 0;
	}

};

#endif