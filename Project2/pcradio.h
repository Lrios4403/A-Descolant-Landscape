#ifndef PCRADIO_H
#define PCRADIO_H

#include <iostream>
#include <string.h>
#include <vector>

#include "fonthandler.h"
#include "program.h"
#include "olcConsoleGameEngine.h"

class PcRadio : public PcProgram
{
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;
	bool bIsFront = false;

	std::vector<PcButton> buttons;

	int Initalize() override
	{
		windowSprite = new olcSprite(250, 75);										//Setup the default window to display

		for (int i = 0; i < windowSprite->nWidth; i++)								//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);

			}

		onTaskbar = true;

		CenterWindow();

		buttons.push_back({25,25,25,5, FontHandler::GetSprite("Exit", BG_GREY | FG_BLACK),std::bind(&PcRadio::Exit, this) });			//Create a basic button

		spriteIcon = new olcSprite(L"resources\\pcradioicon.png.spr");

		return 0;
	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		bIsFront = front;

		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;
		}
		else
			Draw(olc);

		if (IfMouseClick(mouseX, mouseY, olc))
		{
			//Cycle through all the buttons in order to see if we click one
			for (int i = 0; i < buttons.size(); i++)
			{
				if (relMousePosX >= buttons[i].x && relMousePosX <= buttons[i].x + buttons[i].iwidth)						//Test to see if the cursor is inside the button & we havent hovered any other buttons so far
				{
					if (relMousePosY >= buttons[i].y && relMousePosY <= buttons[i].y + buttons[i].iheight)					//...
					{
						buttons[i].function();
						break;
					}
				}
			}
		}
		
		//Check if the mouse is held
		else if (olc->GetMouse(0).bHeld)
		{
			//Test to see if we are dragging the mouse
			if (front)
				if (mouseX != lastMouseX || mouseY != lastMouseY)															//We are dragging
				{
					if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)								//We are moving the window
					{
						isdraggingwindow = true;
						isNeeded = true;
					}
				}
				else																									//We are clicking
				{
					if (mouseX == windowPosition.u + (windowSprite->nWidth - 3) || mouseX == windowPosition.u + (windowSprite->nWidth - 2))
					{
						if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
						{
							exitProgram = true;

							for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
							{
								for (int j = 0; j < windowSprite->nHeight; j++)
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

	if (isdraggingwindow)
	{
		windowPosition.u += mouseX - lastMouseX;
		windowPosition.v += mouseY - lastMouseY;
	}

	return 0;
	}

	int Exit()
	{
		exitProgram = true;

		return 0;
	}

	int Display() override
	{

		for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);

		for (int b = 0; b < buttons.size(); b++)															//Draw and cycle through all the buttons
		{
			for (int i = 0; i < buttons[b].iwidth; i++)														//Draw the sprite
				for (int j = 0; j < buttons[b].iheight; j++)
				{
					windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 2, PIXEL_HALF);				//Set the color of the sprite
					windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 2, BG_GREY | FG_WHITE);
				}
		}

		return 0;
	}

	int Draw(olcConsoleGameEngine* e)
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		for (int i = 0; i < windowSprite->nWidth; i++)															//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);

		//Cycle through all the buttons in order to see if we click one
		bool bInside = false;

		for (int b = 0; b < buttons.size(); b++)
		{
			if (onDisplay)
			if (relMousePosX >= buttons[b].x && relMousePosX <= buttons[b].x + buttons[b].iwidth && !bInside && bIsFront)	//Test to see if the cursor is inside the button & we havent hovered any other buttons so far
			{
				if (relMousePosY >= buttons[b].y && relMousePosY <= buttons[b].y + buttons[b].iheight)						//...
				{
					bInside = true;

					for (int i = 0; i < buttons[b].iwidth; i++)																//Draw the sprite
						for (int j = 0; j < buttons[b].iheight; j++)
						{
							windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 3, PIXEL_HALF);						//Set the color of the sprite
							windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 3, BG_GREY | FG_WHITE);
						}

					int sx = buttons[b].x + (buttons[b].iwidth / 2) - (buttons[b].spriteInside->nWidth/2.5);
					int sy = buttons[b].y + (buttons[b].iheight / 2) - (buttons[b].spriteInside->nHeight / 2.5) + 4;

					AddSpriteToWindow(buttons[b].spriteInside, sx, sy);

					break;
				}

			}
		
			if(!bInside)
			{
				for (int i = 0; i < buttons[b].iwidth; i++)																	//Draw the sprite
					for (int j = 0; j < buttons[b].iheight; j++)
					{
						windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 2, PIXEL_HALF);							//Set the color of the sprite
						windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 2, BG_GREY | FG_WHITE);
					}

				int sx = buttons[b].x + (buttons[b].iwidth / 2) - (buttons[b].spriteInside->nWidth / 2.5);
				int sy = buttons[b].y + (buttons[b].iheight / 2) - (buttons[b].spriteInside->nHeight / 2.5) + 3;

				AddSpriteToWindow(buttons[b].spriteInside, sx, sy);
			}
		}

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