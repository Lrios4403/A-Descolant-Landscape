#ifndef ANTICHEAT_H
#define ANTICHEAT_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <thread>
#include <Psapi.h>

#include "debug.h"

class Anticheat
{
private:
	std::thread mainThread;
	bool bRenderingThread = false;
	bool bFirstRun = false;

public:

	int Initalize()
	{
		odprintf("Initalizing Anticheat instance...");

		odprintf("Initalized Anticheat instance!");

		return 0;
	}

	int StartAnticheat()
	{
		odprintf("Anticheat[%p].StartAnticheat()...", this);

		debugTabIndex++;

		mainThread = std::thread(&Anticheat::MainThread, this);

		if (!mainThread.joinable())
		{
			odprintf("WARNING: mainThread is already running! Returning false...");
			return false;
		}

		mainThread.detach();																		//Detach the thread from the main thread

		while (bRenderingThread == false && bFirstRun == false)
		{
			odprintf("WAITING [%d %d]...", bRenderingThread, bFirstRun);

			Sleep(1);

		}

		debugTabIndex--;

		return 0;
	}

	int MainThread()
	{
		bRenderingThread = true;

		odprintf("THREAD [Anticheat.MainThread()] Called & Running...");

		while (bRenderingThread)
		{
			if (!bFirstRun)
			{

			}
			
			bFirstRun = true;

			Sleep(1000);																					//Sleep for a second
		}

		return 0;
	}

	bool IsRunning() { return bRenderingThread; }

};

#endif