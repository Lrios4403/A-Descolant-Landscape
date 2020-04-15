#ifndef SCENESELECT_H
#define SCENESELECT_H

#include "consoleManager.h"
#include "render.h"
#include "scene.h"
#include "debug.h"

#include <iostream>
#include <vector>
#include <cmath>

class SceneSelect : public Scene
{
private:
	int currentIndex = 0;

	vector<std::wstring> wsScenes = 
	{
		wstring(L"Scene Selector"),
		wstring(L"World View"),
		wstring(L"Sprite Editor"),
		wstring(L"Diary")
	};

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[SceneSelect]...");
		debugTabIndex++;
		this->olcEngine = e;

		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(75, 25, 16, 32);
		ConsolePointerMacro->SetCursorVisible(false);
		ConsolePointerMacro->bCaptureMouse = true;

		odprintf("SCENE Switched to SceneSelect!");

		return true;
	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{

		if (ConsolePointerMacro->bCaptureMouse)
		{
			//Arrow Keys
			if (olcEngine->GetKey(VK_UP).bPressed)		currentIndex = abs(currentIndex - 1);
			if (olcEngine->GetKey(VK_DOWN).bPressed)	currentIndex = abs(currentIndex + 1);
			if (olcEngine->GetKey(VK_RETURN).bPressed)	sceneIndex = currentIndex;

			//Alt Keycodes
			if (olcEngine->GetKey(VK_NEXT).bPressed)
			{
				fDesiredFramerate = std::fabs(fDesiredFramerate - 10.0f);			//PAGE DOWN

				if (ConsolePointerMacro->fFps > 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
				odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));
			}

			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps > 0)		ConsolePointerMacro->fFps = 0;
			else if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps == 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld)											odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));

		}
		else
		{
			ShowCursor(true);
		}

		if (currentIndex > wsScenes.size()-1)currentIndex = wsScenes.size()-1;

		for (int i = 0; i < wsScenes.size(); i++)
		{
			if (currentIndex == i) olcEngine->DrawString(0, i, wsScenes[i], FG_BLACK | BG_WHITE);
			else olcEngine->DrawString(0, i, wsScenes[i], BG_BLACK | FG_WHITE);
		}

		return true;
	}

	bool End() override
	{

		return true;
	}
};

#endif