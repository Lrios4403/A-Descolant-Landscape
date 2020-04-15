#ifndef ENGINE_H
#define ENGINE_H

#include "olcConsoleGameEngine.h"

#include "macro.h"
#include "debug.h"
#include "anticheat.h"
#include "sound.h"
#include "steam.h"
#include "consoleManager.h"
#include "openvr.h"
#include "scene.h"
#include "sceneselect.h"
#include "sceneview.h"
#include "scenespriteeditor.h"
#include "scenediary.h"

#define WINVER			0x0400 
#define _WIN32_WINNT	0x0400 
#include <windows.h>

/*=====================================================================================================================*/
/*
/*	TODO:	+
/*
/*=====================================================================================================================*/

class Engine : public olcConsoleGameEngine
{
public:
	
	std::vector<Scene*> scenes;

	bool bFistRun = false;

	Anticheat anticheat;
	SoundManager soundManager;
	ConsoleManager consoleManager;
	//OpenVrManager ovrManager;

	DWORD dwConsoleColors[16];
	DWORD dwPrevConsoleColors[16];

	Engine()
	{
		m_sAppName = L"World Builder";
	}

	bool OnUserCreate() override
	{
		odprintf("Initalizing Engine...");
		debugTabIndex++;

		ShowCursor(false);

		odprintf("Initalizing basic classes...");
		debugTabIndex++;
		anticheat = Anticheat();
		anticheat.Initalize();
		anticheat.StartAnticheat();
		consoleManager = ConsoleManager();
		consoleManager.Initalize(this);
		soundManager = SoundManager();
		soundManager.Initalize();
		//ovrManager = OpenVrManager();
		//ovrManager.Initalize();
		debugTabIndex--;

		odprintf("Initalizing all the scenes...");
		scenes.push_back(new SceneSelect());
		scenes.push_back(new SceneView());
		scenes.push_back(new SceneSpriteEditor());
		scenes.push_back(new SceneDiary());
		for(int i = 0; i < scenes.size(); i++)
			scenes[i]->Initalize(this);

		debugTabIndex--;
		odprintf("Initalzed Engine!");
		odprintf("");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		long lastIndex = sceneIndex;

		SWITCHANDRUN(bFistRun, consoleManager.ShowConsole(true); scenes[sceneIndex]->Switch(sceneIndex); scenes[sceneIndex]->Update(fElapsedTime, &sceneIndex); consoleManager.CenterWindow();)

		scenes[sceneIndex]->Update(fElapsedTime, &sceneIndex);

		if (sceneIndex != lastIndex)
		{
			scenes[sceneIndex]->Switch(lastIndex);
			scenes[lastIndex]->End();
		}

		if (consoleManager.fFps - fElapsedTime > 0 && consoleManager.fFps != 0) Sleep(consoleManager.fFps - fElapsedTime);

		return true;
	}

	bool OnUserDestroy() override
	{
		odprintf("");
		odprintf("Engine[%p].OnUserDestroy()...", this);

		debugTabIndex++;

		odprintf("Cleaning the scenes...");

		for (auto s : scenes)
		{
			s->End();
		}

		odprintf("Calling class destructors...");
		consoleManager.Destroy();

		odprintf("Exiting...");

		debugTabIndex--;

		return true;
	}

};

#endif