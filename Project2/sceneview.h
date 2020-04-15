#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include "consoleManager.h"
#include "render.h"
#include "scene.h"
#include "tdprocedural.h"
#include "debug.h"

#include <iostream>
#include <vector>
#include <cmath>

class SceneView : public Scene
{
private:
	Render* render;
	std::vector<object*> worldObj;
	olcSprite* screenSprite;
	wstring text[3];

	WaveData* wavBackground1, *wavBackground2, *wavBackground3;

	bool bRenderingThread = true;
	bool bNoClip = false;
	int iWidth, iHeight;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[SceneView]...");
		debugTabIndex++;
		this->olcEngine = e;

		odprintf("Loading sounds...");
		//int iResult = SoundMacro->LoadSoundFile(L"ambeince.wav", true, true, true);
		//if (iResult == 0)
		//{
		//	wavAmbience = SoundMacro->vSounds[SoundMacro->vSounds.size() - 1];
		//	wavAmbience->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		//}
		int iResult = 0;
		SoundMacroLoad(iResult, wavBackground1, L"resources\\background1.wav", true, true, false);
		SoundMacroLoad(iResult, wavBackground2, L"resources\\background2.wav", true, true, false);
		SoundMacroLoad(iResult, wavBackground3, L"resources\\background3.wav", true, true, false);
		wavBackground3->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

		odprintf("Setting up renderer...");
		iWidth = olcEngine->ScreenWidth();
		iHeight = olcEngine->ScreenHeight();
		render = new Render();
		render->Initalize(iWidth, iHeight);
		render->ClearColor = FG_CYAN | BG_RED;
		render->ClearChar = PIXEL_THREEQUARTERS;
		render->bRenderNormals = false;

		LoadHouse();

		odprintf("Allocating sprite memory...");
		screenSprite = new olcSprite(iHeight, iHeight);

		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		ConsolePointerMacro->SetCursorVisible(false);
		ConsolePointerMacro->bCaptureMouse = true;

		olcEngine->ConstructConsole(300, 100, 4, 8);

		render->bHighlightObject = false;
		render->bHighlightTriangle = false;
		render->Resize(300, 100);
		render->RenderStart();
		odprintf("SCENE Switched to SceneView!");

		return true;
	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		//Use the mouse for camera rotation
		if (olcEngine->GetKey(27).bPressed)
		{
			HWND h_console = GetConsoleWindow();
			RECT size; GetWindowRect(h_console, &size);
			int width = size.right - size.left;
			int height = size.bottom - size.top;
			POINT cursorPos;
			ConsolePointerMacro->bCaptureMouse = !ConsolePointerMacro->bCaptureMouse;
			SetCursorPos((width / 2) + size.left, (height / 2) + size.top);
			odprintf("Toggled Mouse Capture: %s", ConsolePointerMacro->bCaptureMouse ? "true" : "false");

		}

		if (ConsolePointerMacro->bCaptureMouse)
		{
			HWND h_console = GetConsoleWindow();
			RECT size; GetWindowRect(h_console, &size);
			int width = size.right - size.left;
			int height = size.bottom - size.top;
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			float yawOffset = (float)((cursorPos.x - ((width / 2) + size.left)) * 0.005);
			float pitchOffset = (float)((cursorPos.y - ((height / 2) + size.top)) * 0.005);

			render->fYaw += yawOffset;
			render->fPitch += pitchOffset;

			if (render->fYaw > TWO_PI) render->fYaw -= TWO_PI;
			if (render->fYaw < -TWO_PI) render->fYaw += TWO_PI;
			if (render->fPitch > HALF_PI) render->fPitch = HALF_PI - 0.001f;
			if (render->fPitch < -HALF_PI) render->fPitch = -HALF_PI + 0.001f;

			SetCursorPos((width / 2) + size.left, (height / 2) + size.top);

			//Basic 3D movements
			if (bRenderingThread)
			{
				if (bNoClip)
				{
					if (olcEngine->GetKey(VK_UP).bHeld)
						render->map.vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

					if (olcEngine->GetKey(VK_DOWN).bHeld)
						render->map.vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards

					// Dont use these two in FPS mode, it is confusing :P
					if (olcEngine->GetKey(VK_LEFT).bHeld)
						render->map.vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

					if (olcEngine->GetKey(VK_RIGHT).bHeld)
						render->map.vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
					///////
				}

				vec3d vForward = Vector_Mul(render->vLookDir, 8.0f * fElapsedTime);

				// Standard FPS Control scheme, but turn instead of strafe
				if (olcEngine->GetKey(L'W').bHeld)
					render->map.vCamera = Vector_Add(render->map.vCamera, vForward);

				if (olcEngine->GetKey(L'S').bHeld)
					render->map.vCamera = Vector_Sub(render->map.vCamera, vForward);

				if (olcEngine->GetKey(L'A').bHeld)
					render->fYaw -= 2.0f * fElapsedTime;

				if (olcEngine->GetKey(L'D').bHeld)
					render->fYaw += 2.0f * fElapsedTime;

				if (olcEngine->GetKey(L'N').bPressed) bNoClip != bNoClip;

				if (olcEngine->GetKey(L'R').bPressed) LoadHouse();

				//if (!bNoClip) render->map.vCamera.y = 3.3f;

				if (olcEngine->GetKey(VK_PRIOR).bPressed)
				{
					fDesiredFramerate = std::fabs(fDesiredFramerate + 10.0f);			//PAGE UP

					if (ConsolePointerMacro->fFps > 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
					if (render->fFps > 0)	render->fFps = 1000.0f / fDesiredFramerate;
					odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));
					odprintf("Render FPS: %f %f", (1000.0f / render->fFps), render->lLastElapsedFPS);
				}
			}

			//Tab capture
			if (olcEngine->GetKey(VK_HOME).bPressed)
			{
				render->RenderStop();
				sceneIndex = 0;
			}

			//Alt Keycodes
			if (olcEngine->GetKey(VK_NEXT).bPressed)
			{
				fDesiredFramerate = std::fabs(fDesiredFramerate - 10.0f);			//PAGE DOWN

				if (ConsolePointerMacro->fFps > 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
				if (render->fFps > 0)	render->fFps = 1000.0f / fDesiredFramerate;
				odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));
				odprintf("Render FPS: %f %f", (1000.0f / render->fFps), render->lLastElapsedFPS);
			}

			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps > 0)		ConsolePointerMacro->fFps = 0;
			else if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps == 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld)											odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));

			if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld && render->fFps > 0)						render->fFps = 0;
			else if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld && render->fFps == 0)				render->fFps = 1000.0f / fDesiredFramerate;
			if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld)											odprintf("Render FPS: %f", (1000.0f / render->fFps));

		}
		else
		{
			ShowCursor(true);

		}

		//Draw the sprite
		if (bRenderingThread)
		{
			render->CopySprite(screenSprite);
			olcEngine->DrawSprite(0, 0, screenSprite);

			int tempw = ConsolePointerMacro->GetTerminalSize().x;
			int temph = ConsolePointerMacro->GetTerminalSize().y;
			if (tempw != iWidth || temph != iHeight)
			{
				odprintf("Screen has been resized[%i, %i]! Resizing....", olcEngine->ScreenWidth(), olcEngine->ScreenHeight());
				iWidth = tempw;
				iHeight = temph;

				olcEngine->ConstructConsole(tempw, temph, 4, 8);
				render->Resize(iWidth, iHeight);
				screenSprite = new olcSprite(iWidth, iHeight);

			}

		}
		else
		{
			render->CopySprite(screenSprite);

			POINT sizep = ConsolePointerMacro->GetTerminalSize();

			float fxoffset = screenSprite->nWidth / sizep.x;
			float fyoffset = screenSprite->nHeight / sizep.y;

			for (int y = 0; y < sizep.y; y++)
				for (int x = 0; x < sizep.x; x++)
					olcEngine->Draw(x, y, screenSprite->GetGlyph(x* fxoffset, y* fyoffset), screenSprite->GetColour(x* fxoffset, y* fyoffset));

			for (int y = 0; y < 3; y++)
				for (int x = 0; x < sizep.x; x++)
					olcEngine->Draw(x, y, PIXEL_HALF, BG_CYAN | FG_CYAN);

		}

		return true;
	}

	bool End() override
	{
		render->RenderStop();
		return true;
	}

	bool LoadHouse()
	{
		odprintf("Loading world data...");
		render->map.objects.clear();
		render->map.objects.shrink_to_fit();
		render->map.Initalize("house.obj");
		//render->map.LoadFloors("floor.obj");
		//render->map.LoadWalls("walls.obj");

		return true;
	}

};

#endif