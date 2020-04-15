#ifndef COMPUTER_H
#define COMPUTER_H

#include <iostream>
#include <algorithm>
#include <string.h>
#include <vector>

#include "olcConsoleGameEngine.h"
#include "object.h"
#include "tdmath.h"
#include "debug.h"

#include "program.h"
#include "pchelloworld.h"
#include "pcradio.h"

class Computer
{
private:
	int screenWidth= 0, screenHeight= 0, screenFontWidth= 1, screenFontHeight = 1;
	POINT pMouse, pLastMouse;

	olcSprite* spriteScreen;

	vector<PcProgram*> vPrograms;

public:
	int Initalize(olcConsoleGameEngine* e, int fontwidth, int fontheight)
	{
		odprintf("Initalizing Computer...");
		debugTabIndex++;

		odprintf("Setting screen values....");
		screenWidth = e->ScreenWidth();												//Get and set the screen width
		screenHeight = e->ScreenHeight();											//Get and set the screen height

		odprintf("Setting up the computer screen sprite...");
		spriteScreen = new olcSprite(screenWidth, screenHeight);					//Set the screen sprite

		//First we need to create the classes
		odprintf("Creating Program classes...");
		vPrograms.push_back(new PcHelloWorld());
		vPrograms.push_back(new PcRadio());

		int iTaskbarIndex = 0;
		for (PcProgram* p : vPrograms)												//Cycle through all of the programs
		{
			p->Initalize();

			p->Boot(e, 0.0f, 0.0f);

			p->onDisplay = true;													//DEBUG: simple on

			p->pDesktop = { 1, iTaskbarIndex+1 };

			if (p->onTaskbar)														//If the program is on the taskbar
			{
				p->iTaskbarIndex = iTaskbarIndex;
				p->iDesktopIndex = iTaskbarIndex;
				iTaskbarIndex++;
			}
		}																			

		odprintf("Screen dimensions [%ld, %ld]", screenFontWidth, screenFontHeight);

		//Then update the screen
		Update(e, 0.0f, 0.0f);

		debugTabIndex--;
		odprintf("Initalized Computer...");

		return 0;
	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal)
	{
		//Get the mouse position
		RECT rWindow;
		GetCursorPos(&pMouse);
		GetWindowRect(GetConsoleWindow(), &rWindow);

		pLastMouse = pMouse;

		pMouse.x -= rWindow.left + 8;
		pMouse.y -= rWindow.top + 30;

		pMouse.x = round(pMouse.x / 4);
		pMouse.y = round(pMouse.y / 8);

		if (pMouse.x < 0) pMouse.x = 0;												//Set into bounds
		if (pMouse.y < 0) pMouse.y = 0;
		if (pMouse.x > rWindow.right - rWindow.left) pMouse.x = rWindow.right - rWindow.left;
		if (pMouse.y > rWindow.bottom - rWindow.top) pMouse.y = rWindow.bottom - rWindow.top;

		//Check if the index was updated
		if (!vPrograms[0]->isNeeded)												//Test to see if the user requires the first program
		if (olc->GetMouse(0).bPressed || olc->GetMouse(0).bHeld)					//Test to see if the mouse was clicked
		{
			int i = 0;
			for (PcProgram* p : vPrograms)
			{
				if (!p->onDisplay) continue;										//Test to see if its ondisplay

				int posx1 = p->windowPosition.u;									//Get bounds of the program's window
				int posy1 = p->windowPosition.v;
				int posx2 = p->windowPosition.u + p->windowSprite->nWidth;
				int posy2 = p->windowPosition.v + p->windowSprite->nHeight;

				if (pMouse.x >= posx1 && pMouse.x <= posx2)							//Test the bounds of the program v. the mouse
				{
					if (pMouse.y >= posy1 && pMouse.y <= posy2)
					{
						PcProgram* pp = vPrograms[0];								//Save the current program to swap (hahahahahhahhahaahahahahahahhahahahahahhahah weeeeeeeeeeeeeeeeeeeeeeeeeeze)

						vPrograms[0] = p;
						vPrograms[i] = pp;

						break;
					}
				}

				i++;

			}
		}

		//Update all of the vPrograms
		bool bFrontProgram = true;
		for (PcProgram* p : vPrograms)
		{
			p->MouseUpdate(pMouse.x, pMouse.y);
			p->Update(olc, fElapsed, fElapsedTotal, bFrontProgram);
			if (p->onDisplay)bFrontProgram = false;
		}

		vector<PcProgram*> tp = GetTaskbarIndex();																			//Get the programs in order 
			
		if (olc->GetMouse(0).bPressed)																						//If we clicked something
		{	
																															//Test to see if we clicked on a window on the taskbar
			int xoffset = 0, i = 0;
			float toolbaroffset = screenHeight * 0.95;																		//The y pointer to the bottom of the screen

			if (pMouse.y >= toolbaroffset)																					//If the mouse y is in the taskbar
				for (PcProgram* p : tp)																						//If the program is on the taskbar
				{
					if (p->onTaskbar)																						//Test to see if the program is on the taskbar
					{
						xoffset++;																							//Addon to where we need to draw

						int xo = (xoffset * 13) + 36;

						if (pMouse.x >= xo && pMouse.x <= xo + 13)															//The mouse clicked the program on the taskbar
						{
							if(!p->onDisplay)
							p->Boot(olc, fElapsed, fElapsedTotal);

							//odprintf("%i | %i | %i | %p", xo, xo+13, pMouse.x, p);

							if (p != vPrograms[0])																			//If the program isnt already in front
							{																								//Then set it to the front
								auto f = std::find(vPrograms.begin(), vPrograms.end(), p);									//Find the current index
								auto d = std::distance(vPrograms.begin(), f);												//Get the distance (always int)

								std::swap(vPrograms[0], vPrograms[d]);														//Swap them

							}
						}
						
						i++;

						//spriteScreen->SetGlyph((xoffset * 13) + 36, toolbaroffset + j, ch);
						//spriteScreen->SetColour((xoffset * 13) + 36, toolbaroffset + j, col);

					}
				}
		}

		//Render stuff
		RenderDesktop();
		RendervPrograms();
		RenderToolbar();
		RenderFrontProgram();														//Render the front program above the toolbar

		//Render the cursor to the program
		spriteScreen->SetGlyph(pMouse.x, pMouse.y, PIXEL_HALF);
		spriteScreen->SetColour(pMouse.x, pMouse.y, BG_BLACK | FG_WHITE);

		return 0;
	}

	int RenderDesktop()
	{
		//Render the cyan backgrounds
		for (int i = 0; i < screenWidth; i++)
			for (int j = 0; j < screenHeight; j++)
			{
				spriteScreen->SetGlyph(i, j, PIXEL_HALF);
				spriteScreen->SetColour(i, j, BG_DARK_CYAN | FG_CYAN);
			}

		void* lastp = nullptr;
		for (int i = 0; i < vPrograms.size(); i++)
		{
			PcProgram* p = vPrograms[i];

			if (p->pDesktop.x != 0 && p->pDesktop.y != 0)
			{
				int tx = (p->pDesktop.x-1) * 12 + 2;
				int ty = (p->pDesktop.y-1) * 8 + 1;

				//odprintf("%i %i %i %i %i ", tx, ty, p->pDesktop.x, p->pDesktop.y, i);

				for (int i = 0; i < 12; i++)																		//Draw the icon
					for (int j = 0; j < 8; j++)
					{
						int xpos = tx + i;
						int ypos = ty + j;

						short col, ch;
						col = p->spriteIcon->GetColour(i, j);
						ch = p->spriteIcon->GetGlyph(i, j);

						if (ch == 32) continue;																		//Test to see if the character is non-existant

						spriteScreen->SetColour(xpos, ypos, col);
						spriteScreen->SetGlyph(xpos, ypos, ch);
					}

			}
		}

		return 0;
	}

	int RenderToolbar()
	{
		float toolbaroffset = screenHeight * 0.95;																	//The y pointer to the bottom of the screen

		for (int i = 0; i < screenWidth; i++)																		//Draw the main taskbar
			for (int j = 0; j < screenHeight * 0.05; j++)
				spriteScreen->SetColour(i, toolbaroffset + j, BG_CYAN | FG_CYAN);

		for (int i = 0; i < 12; i++)																				//Draw the entire bar
			for (int j = 0; j < 8; j++)
			{
				spriteScreen->SetGlyph(i+1, toolbaroffset + j, PIXEL_QUARTER);
				spriteScreen->SetColour(i+1, toolbaroffset + j, BG_DARK_GREY | FG_CYAN);
			}

		for (int i = 0; i < 10; i++)																				//Draw the icon
			for (int j = 0; j < 4; j++)
			{
				short col = BG_DARK_RED | FG_RED;

				if (j < 2)
					if (i < 5) col = BG_DARK_RED | FG_RED;
					else col = BG_GREEN | FG_DARK_GREEN;
				else
					if (i < 5) col = BG_DARK_BLUE | FG_BLUE;
					else col = BG_DARK_YELLOW | FG_YELLOW;

				int y = toolbaroffset + j;

				if (i > 4) y++;

				spriteScreen->SetGlyph(i + 2, y, PIXEL_THREEQUARTERS);
				spriteScreen->SetColour(i + 2, y, col);
			}

		int xoffset = 0;

		vector<PcProgram*> tp = GetTaskbarIndex();

		for (PcProgram* p : tp)																						//If the program is on the taskbar
		{
			if (p->onTaskbar)																						//Test to see if the program is on the taskbar
			{
				xoffset++;																							//Addon to where we need to draw

				for (int i = 0; i < 12; i++)																		//Draw the icon
					for (int j = 0; j < 8; j++)
					{
						if (i >= p->spriteIcon->nWidth) continue;
						if (j >= p->spriteIcon->nHeight) continue;

						short col, ch;
						col = p->spriteIcon->GetColour(i, j);
						ch = p->spriteIcon->GetGlyph(i, j);

						if (ch == 32) continue;																		//Test to see if the character is non-existant

						spriteScreen->SetGlyph((i + 1) + (xoffset * 13) + 36, toolbaroffset + j, ch);
						spriteScreen->SetColour((i + 1) + (xoffset * 13) + 36, toolbaroffset + j, col);
					}
			}
		}

		return 0;
	}

	int RendervPrograms()
	{
		//Now we must append the vPrograms window sprites to the main sprites
		for (int i = 0; i < vPrograms.size(); i++)
		{
			//Check if we should display it.
			if (vPrograms[i]->onDisplay == false)
				continue;

			//Now we must copy the sprite over to the mainsprite
			for (int w = 0; w < vPrograms[i]->windowSprite->nWidth; w++)
				for (int h = 0; h < vPrograms[i]->windowSprite->nHeight; h++)
				{
					//Get the x & y position to set the window
					int x = vPrograms[i]->windowPosition.u + w;
					int y = vPrograms[i]->windowPosition.v + h;

					//Check the bounds of the clip
					if (x < 0 || y < 0) continue;
					if (x > screenWidth || y > screenHeight) continue;

					//Set the pixel
					short col = (vPrograms[i]->windowSprite->GetColour(w, h) >> 1) & 0xff | BG_GREY;

					spriteScreen->SetColour(x, y, col);
					spriteScreen->SetGlyph(x, y, PIXEL_HALF);
				}
		}

		return 0;
	}

	int RenderFrontProgram()
	{
		//Now we must append the vPrograms window sprites to the main sprites
		for (int i = 0; i < vPrograms.size(); i++)
		{
			//Check if we should display it.
			if (vPrograms[i]->onDisplay == false)
				continue;

			//Now we must copy the sprite over to the mainsprite
			for (int w = 0; w < vPrograms[i]->windowSprite->nWidth; w++)
				for (int h = 0; h < vPrograms[i]->windowSprite->nHeight; h++)
				{
					//Get the x & y position to set the window
					int x = vPrograms[i]->windowPosition.u + w;
					int y = vPrograms[i]->windowPosition.v + h;

					//Check the bounds of the clip
					if (x < 0 || y < 0) continue;
					if (x > screenWidth || y > screenHeight) continue;

					//Set the pixel
					spriteScreen->SetColour(x, y, vPrograms[i]->windowSprite->GetColour(w, h));
					spriteScreen->SetGlyph(x, y, vPrograms[i]->windowSprite->GetGlyph(w, h));
				}

			break;

		}

		return 0;
	}

	vector<PcProgram*> GetTaskbarIndex()
	{

		vector<PcProgram*> tp = vPrograms;

		std::sort(tp.begin(), tp.end(), [](PcProgram* t1, PcProgram* t2)											//Sort the taskbar index
			{
				return t1->iTaskbarIndex < t2->iTaskbarIndex;
			});

		return tp;
	}

	olcSprite* GetScreenSprite()
	{
		return spriteScreen;
	}

};

#endif