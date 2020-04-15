#ifndef FONTHANDLER_H
#define FONTHANDLER_H

#include "olcConsoleGameEngine.h"
#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <exception>

using namespace std;

class FontHandler
{
public:

	static olcSprite* GetSprite(std::string str, short col)
	{
		char asciiTable[1][32] =
		{
			"abcdefghijklmnopqrstuvwxyz",
		};

		wchar_t spriteTable[4][321] =
		{
			L"▄▀▄ █▀▄ ▄▀▀ █▀▄ █▀▀ █▀▀ █▀▀ █ █ ▀█▀ ▀▀█ █ █ █   █▄█ █▌█ █▀█ █▀█ █▀█ █▀█ ▄▄▄ ▀█▀ █ █ █ █ ▄  ▄ ▀▄▀ █ █ ▄▄▄    ",
			L"█▀█ █▀▄ ▀▄▄ █▄▀ █▀  █▀  ■▄■ █▀█ ▄█▄ ▄▄■ █▀▄ █▄▄ █ █ █▐█ █▄█ ▌▀▀ ▀▀■ █▀▄ ▀■▄  █  ▀▀■ ▀■▀ █▐▌█ ▄▀▄  █  ▄■▀    ",
			L"     ▀          ▀▀▀                                                     ▀▀▀              ▀▀          ▀▀▀    "
		};

		int cw = 1, w = 1, h = 1, x = 0, y = 0;

		string s = str;

		transform(s.begin(), s.end(), s.begin(), ::toupper);

		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] == '\n')
			{
				h++;
				cw = 1;
			}
			else
			{
				cw++;
				if (cw > w) w = cw;
			}
		}

		olcSprite* sprite = new olcSprite(w * 4, h * 4);

		for (int j = 0; j < sprite->nWidth; j++)
			for (int k = 0; k < sprite->nHeight; k++)
				sprite->SetColour(j, k, col);

		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] == '\n')
			{
				y++;
				x = 0;
			}
			else
			{
				int pos = s[i] - 'A';

				if (pos > 25) pos -= 6;

				if (s[i] == ' ') pos = 26;

				for(int j = 0; j < 4; j++)
					for (int k = 0; k < 3; k++)
					{
						int xpos = (x * 4) + j, ypos = (y * 4) + k;
						wchar_t c = spriteTable[k][(pos * 4) + j];

						sprite->SetGlyph(xpos, ypos, c);

					}

				x++;
			}
		}

		return sprite;
	}
};

#endif