#include "engine.h"

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	Engine engine;

	//if (engine.ConstructConsole(300, 200, 4, 4))
	//if (engine.ConstructConsole(640,200, 8 * 1.25 / 2 / 2, 16 * 1.25 / 2 / 2))
	//if (engine.ConstructConsole(80 * 2 * 2*1.75, 25 * 2 * 2*1.75, 8 * 1.25 / 2 / 2, 16 * 1.25 / 2 / 2))
	//if (engine.ConstructConsole(80, 25, 8, 16))
	//if (engine.ConstructConsole(80 * 2 * 2 * 1.5, 25 * 2 * 2 * 1.5, 8 * 1.25 / 2 / 2, 8 * 1.25 / 2 / 2*2))
	//if (engine.ConstructConsole(300,200,4,4))
	if (engine.ConstructConsole(80,25,8,16))
	//if (engine.ConstructConsole(600,200,2,4))
		engine.Start();
	else
	{
		std::cout << "ERROR: Could not set the resize of the console! \n";
		return -1;
	}
	

	return 0;
}