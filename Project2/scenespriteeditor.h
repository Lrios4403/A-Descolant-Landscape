#ifndef SCENESPRITEEDITOR_H
#define SCENESPRITEEDITOR_H

#include "consoleManager.h"
#include "render.h"
#include "scene.h"
#include "debug.h"

#include <iostream>
#include <vector>
#include <cmath>

class SceneSpriteEditor : public Scene
{
private:
	olcSprite* screenSprite = nullptr;
	olcSprite* spriteRGB = nullptr;

	long lSelectedTriangle = 0;
	long lSlectedObject = 0;
	bool bRenderingThread = true;
	float xoffset = 0, yoffset = 0;


public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[SceneSpriteEditor]...");
		debugTabIndex++;
		this->olcEngine = e;

		odprintf("Allocating sprite memory...");
		screenSprite = new olcSprite(300, 100);
		spriteRGB = new olcSprite(L"resources\\RGB_24bits_palette_color_test_chart.png.spr");

		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(300, 100, 4, 8);
		ConsolePointerMacro->SetCursorVisible(false);
		ConsolePointerMacro->bCaptureMouse = true;

		RenderersPointerMacro(0)->bHighlightObject = true;
		RenderersPointerMacro(0)->bHighlightTriangle = true;
		RenderersPointerMacro(0)->RenderStart();

		odprintf("SCENE Switched to SceneSpriteEditor!");

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

			RenderersPointerMacro(0)->fYaw += yawOffset;
			RenderersPointerMacro(0)->fPitch += pitchOffset;

			if (RenderersPointerMacro(0)->fYaw > TWO_PI) RenderersPointerMacro(0)->fYaw -= TWO_PI;
			if (RenderersPointerMacro(0)->fYaw < -TWO_PI) RenderersPointerMacro(0)->fYaw += TWO_PI;
			if (RenderersPointerMacro(0)->fPitch > HALF_PI) RenderersPointerMacro(0)->fPitch = HALF_PI - 0.001f;
			if (RenderersPointerMacro(0)->fPitch < -HALF_PI) RenderersPointerMacro(0)->fPitch = -HALF_PI + 0.001f;

			SetCursorPos((width / 2) + size.left, (height / 2) + size.top);

			//Basic 3D movements
			if (bRenderingThread)
			{
				if (olcEngine->GetKey(VK_UP).bHeld)
					RenderersPointerMacro(0)->map.vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

				if (olcEngine->GetKey(VK_DOWN).bHeld)
					RenderersPointerMacro(0)->map.vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards

				// Dont use these two in FPS mode, it is confusing :P
				if (olcEngine->GetKey(VK_LEFT).bHeld)
					RenderersPointerMacro(0)->map.vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

				if (olcEngine->GetKey(VK_RIGHT).bHeld)
					RenderersPointerMacro(0)->map.vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
				///////


				vec3d vForward = Vector_Mul(RenderersPointerMacro(0)->vLookDir, 8.0f * fElapsedTime);

				// Standard FPS Control scheme, but turn instead of strafe
				if (olcEngine->GetKey(L'W').bHeld)
					RenderersPointerMacro(0)->map.vCamera = Vector_Add(RenderersPointerMacro(0)->map.vCamera, vForward);

				if (olcEngine->GetKey(L'S').bHeld)
					RenderersPointerMacro(0)->map.vCamera = Vector_Sub(RenderersPointerMacro(0)->map.vCamera, vForward);

				if (olcEngine->GetKey(L'A').bHeld)
					RenderersPointerMacro(0)->fYaw -= 2.0f * fElapsedTime;

				if (olcEngine->GetKey(L'D').bHeld)
					RenderersPointerMacro(0)->fYaw += 2.0f * fElapsedTime;

				if (olcEngine->GetKey(VK_PRIOR).bPressed)
				{
					fDesiredFramerate = std::fabs(fDesiredFramerate + 10.0f);			//PAGE UP

					if (ConsolePointerMacro->fFps > 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
					if (RenderersPointerMacro(0)->fFps > 0)	RenderersPointerMacro(0)->fFps = 1000.0f / fDesiredFramerate;
					odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));
					odprintf("Render FPS: %f %f", (1000.0f / RenderersPointerMacro(0)->fFps), RenderersPointerMacro(0)->lLastElapsedFPS);
				}
			}
			else
			{
				if (olcEngine->GetKey(VK_UP).bHeld)		yoffset += fElapsedTime * 30.0f;

				if (olcEngine->GetKey(VK_DOWN).bHeld)	yoffset -= fElapsedTime * 30.0f;

				if (olcEngine->GetKey(VK_LEFT).bHeld)	xoffset += fElapsedTime * 30.0f;

				if (olcEngine->GetKey(VK_RIGHT).bHeld)	xoffset -= fElapsedTime * 30.0f;

			}

			//Home capture
			if (olcEngine->GetKey(VK_HOME).bPressed)
			{
				RenderersPointerMacro(0)->RenderStop();
				sceneIndex = 0;
				bRenderingThread = false;
			}

			if (olcEngine->GetKey(VK_TAB).bPressed)
			{
				bRenderingThread = !bRenderingThread;

				odprintb(bRenderingThread);

				if (bRenderingThread)
				{
					RenderersPointerMacro(0)->RenderStop();
				}
				else
				{
					RenderersPointerMacro(0)->RenderStop();
				}
			}

			//Alt Keycodes
			if (olcEngine->GetKey(VK_NEXT).bPressed)
			{
				fDesiredFramerate = std::fabs(fDesiredFramerate - 10.0f);			//PAGE DOWN

				if (ConsolePointerMacro->fFps > 0)	ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
				if (RenderersPointerMacro(0)->fFps > 0)	RenderersPointerMacro(0)->fFps = 1000.0f / fDesiredFramerate;
				odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));
				odprintf("Render FPS: %f %f", (1000.0f / RenderersPointerMacro(0)->fFps), RenderersPointerMacro(0)->lLastElapsedFPS);
			}

			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps > 0)						ConsolePointerMacro->fFps = 0;
			else if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld && ConsolePointerMacro->fFps == 0)					ConsolePointerMacro->fFps = 1000.0f / fDesiredFramerate;
			if (olcEngine->GetKey(VK_NUMPAD0).bPressed && olcEngine->GetKey(VK_MENU).bHeld)															odprintf("Main FPS: %f", (1000.0f / ConsolePointerMacro->fFps));

			if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld && RenderersPointerMacro(0)->fFps > 0)					RenderersPointerMacro(0)->fFps = 0;
			else if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld && RenderersPointerMacro(0)->fFps == 0)				RenderersPointerMacro(0)->fFps = 1000.0f / fDesiredFramerate;
			if (olcEngine->GetKey(VK_NUMPAD1).bPressed && olcEngine->GetKey(VK_MENU).bHeld)															odprintf("Render FPS: %f", (1000.0f / RenderersPointerMacro(0)->fFps));



		}
		else
		{
			ShowCursor(true);
		}

		//Draw the sprite
		if (bRenderingThread)
		{
			RenderersPointerMacro(0)->CopySprite(screenSprite);
			olcEngine->DrawSprite(0, 0, screenSprite);
		}
		else
		{
			for (int x = 0; x < 300; x++)
				for (int y = 0; y < 200; y++)
				{
					olcEngine->Draw(x, y, PIXEL_HALF, FG_BLACK | BG_WHITE);
				}

			olcEngine->DrawSprite((int)xoffset, (int)yoffset, spriteRGB);
		}

		return true;
	}

	bool End() override
	{

		return true;
	}
};

#endif