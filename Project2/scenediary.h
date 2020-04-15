#ifndef SCENEDIARY_H
#define SCENEDIARY_H

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>

#include "olcConsoleGameEngine.h"
#include "consoleManager.h"
#include "string.h"
#include "render.h"
#include "scene.h"
#include "debug.h"

class SceneDiary : public Scene
{
private:
	olcSprite* sprDiary;
	vector<wstring> vwsDiary;
	bool bDiaryAvaliable = false;
	int dIndex = 0;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[SceneDiary]...");
		debugTabIndex++;
		this->olcEngine = e;

		odprintf("Loading background sprite...");
		sprDiary = new olcSprite(L"resources\\diary.png.spr");

		odprintf("Loading diary enteries...");

		//First open the file
		std::wifstream wif("resources\\diary.txt");
		wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
		std::wstringstream wss;
		wss << wif.rdbuf();
		wstring ws(wss.str());

		vwsDiary = splitwstring(ws, L'¶');

InitEnd:
		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(75, 25, 16, 32);
		ConsolePointerMacro->SetCursorVisible(false);
		ConsolePointerMacro->bCaptureMouse = true;

		Redisplay();

		odprintf("SCENE Switched to SceneDiary!");
		
		return true;
	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{

			if (olcEngine->GetKey(VK_LEFT).bPressed)	dIndex--;

			if (olcEngine->GetKey(VK_RIGHT).bPressed)	dIndex++;

			if (dIndex < 0) dIndex = 0;

			Redisplay();

		return true;
	}

	bool Redisplay()
	{
		//Draw the sprite
		olcEngine->DrawSprite(0, 0, sprDiary);

		int cdIndex = dIndex * 2;

		if (vwsDiary.size() > cdIndex)
		{
			//Split the first page into lines
			vector<wstring> firstpage = splitwstring(vwsDiary[cdIndex], L'\n');

			for (int y = 0; y < firstpage.size(); y++)
			{
				for (int x = 0; x < firstpage[y].size(); x++)
				{
					short col = FG_BLACK | BG_WHITE;
					if (firstpage[y][x] != L'\t' || firstpage[y][x] != L' ') olcEngine->Draw(x + 3, y, firstpage[y][x], col);
				}
			}
		}

		if (vwsDiary.size() > cdIndex + 1)
		{
			vector<wstring> secondpage = splitwstring(vwsDiary[cdIndex + 1], L'\n');

			for (int y = 0; y < secondpage.size(); y++)
			{
				for (int x = 0; x < secondpage[y].size(); x++)
				{
					short col = FG_BLACK | BG_WHITE;
					if (secondpage[y][x] != L'\t' || secondpage[y][x] != L' ') olcEngine->Draw(x + 37, y, secondpage[y][x], col);
				}
			}
		}

		return true;
	}

	bool End() override
	{

		return true;
	}
};

#endif