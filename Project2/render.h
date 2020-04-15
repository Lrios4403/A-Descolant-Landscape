#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <condition_variable>

#include "olcConsoleGameEngine.h"
#include "tdmap.h"
#include "sprite.h"
#include "tdmath.h"
#include "debug.h"
#include "consoleManager.h"

///OPENCL DATA///
#include "opencl.h"

using namespace std;

struct triangle_roastered
{
	olcSprite* sprite;

	vec3d p[3];
	vec2d t[3]; // added a texture coord per vertex
	wchar_t sym;
	short col;
	long triid = 0;
	long objid = 0;
};

triangle_roastered TriToRoastered(triangle t)
{
	triangle_roastered ret;
	ret.sprite = t.spritePtr;
	ret.p[0] = t.p[0];
	ret.p[1] = t.p[1];
	ret.p[2] = t.p[2];
	ret.t[0] = t.t[0];
	ret.t[1] = t.t[1];
	ret.t[2] = t.t[2];
	ret.sym = t.sym;
	ret.col = t.col;

	return ret;
}

triangle RoasteredToTri(triangle_roastered t)
{
	triangle ret;
	ret.spritePtr = t.sprite;
	ret.p[0] = t.p[0];
	ret.p[1] = t.p[1];
	ret.p[2] = t.p[2];
	ret.t[0] = t.t[0];
	ret.t[1] = t.t[1];
	ret.t[2] = t.t[2];
	ret.sym = t.sym;
	ret.col = t.col;

	return ret;
}

std::vector<void*> renderers;
#define RenderersPointerMacro(x) ((Render*)renderers[x])

class Render
{
public:
	///MAIN RENDER DATA///
	CHAR_INFO* screenBuffer;
	short screenClearChar;
	int screenClearColor;
	olcSprite* screenSprite;
	olcSprite* screenSpriteBuffer[2];
	std::chrono::time_point<std::chrono::system_clock> tp1, tp2;
	std::thread mainThread;
	bool bPause = false;

	///MAIN CAMERA DATA///
	mat4x4 matProj;	// Matrix that converts from view space to screen space
	vec3d vLookDir;	// Direction vector along the direction camera points
	float fYaw;		// FPS Camera rotation in XZ plane
	float fPitch;
	float fTheta;	// Spins World transform
	float* pDepthBuffer = nullptr;
	bool bRenderSpecificObject = false;
	int iRenderSpecificObject = 0;
	float fFps = 0;
	long lElapsedFPS = 0;
	long lLastElapsedFPS = 0;
	float fTimeElapsedFPS = 0;
	float fElapsedTotal = 0.0f;
	float fFovOffset = 0.0f;
	bool bMustRender = false;
	bool bWFNFToggle = false;
	bool bHighlightTriangle = false;
	bool bHighlightObject = false;
	bool bRenderNormals = true;
	long lHighlightTriangle = 0;
	long lHighlightObject = 0;
	bool bRenderEnded = false;
	bool bInitalized = false;
	float fFOV = 90;

	///MAIN WORLD DATA///
	TDMap map;

	///SCREEN DATA///
	int screenWidth, screenHeight;
	short ClearColor = FG_CYAN | BG_BLUE;
	short ClearChar = PIXEL_HALF;
	
	float offset, fTotalTime;

private:
	bool bRenderingThread = false;

public:
	int Initalize(int width, int height)
	{
		odprintf("Initalizing renderer instance [%d, %d]...!\n", width, height);
		debugTabIndex++;

		odprintf("Saving system varaibles...!\n");

		this->screenWidth = width;
		this->screenHeight = height;

		fTotalTime = 0;

		odprintf("Creating screen buffer [%d]...!\n", (screenWidth * screenHeight));

		pDepthBuffer = new float[(float)width * height];	//Setup the depth buffer

		// Projection Matrix
		matProj = Matrix_MakeProjection(90, ((float)width/2) / ((float)height * 2), 0.1f, 1000.0f);

		odprintf("Creating screen sprite [%d]...!\n", (screenWidth * screenHeight));

		screenSpriteBuffer[0] = new olcSprite(screenWidth, screenHeight);
		screenSpriteBuffer[1] = new olcSprite(screenWidth, screenHeight);

		screenSprite = screenSpriteBuffer[0];

		if(!bInitalized)
		renderers.push_back(this);

		bInitalized = true;
		debugTabIndex--;
		odprintf("Initalized renderer instance!");

		return 1;
	}

	int Resize(int width, int height)
	{
		odprintf("Initalizing renderer instance [%d, %d]...!\n", width, height);
		debugTabIndex++;

		odprintf("Saving system varaibles...!\n");

		this->screenWidth = width;
		this->screenHeight = height;

		fTotalTime = 0;

		odprintf("Creating screen buffer [%d]...!\n", (screenWidth * screenHeight));

		pDepthBuffer = new float[(float)width * height];	//Setup the depth buffer

		// Projection Matrix
		matProj = Matrix_MakeProjection(90, ((float)width / 2) / ((float)height * 2), 0.1f, 1000.0f);

		odprintf("Creating screen sprite [%d]...!\n", (screenWidth * screenHeight));

		screenSpriteBuffer[0] = new olcSprite(screenWidth, screenHeight);
		screenSpriteBuffer[1] = new olcSprite(screenWidth, screenHeight);

		screenSprite = screenSpriteBuffer[0];

		debugTabIndex--;
		odprintf("Initalized renderer resized!");

		return true;
	}

	int Update(float fElapsedTime)
	{

		return 1;
	}

public:

	bool RenderStart()
	{
		odprintf("Render[%p].RenderStart()...", this);

		debugTabIndex++;

		odprintf("Creating thread for constant rendering...!\n");

		mainThread = std::thread(&Render::RenderCPU, this);

		if (!mainThread.joinable())
		{
			odprintf("WARNING: mainThread is already running! Returning false...");
			return false;
		}

		mainThread.detach();																		//Detach the thread from the main thread

		while (!bRenderingThread)
		{
			odprintf("WAITING [%d]...", bRenderingThread);

			Sleep(1);

		}

		debugTabIndex--;

		return true;
	}

	bool RenderStop()
	{
		bRenderingThread = false;
		return true;
	}

	bool RenderCPU()
	{
		tp1 = std::chrono::system_clock::now();														//Set the internal clock

		bRenderingThread = true;																	//Set the boolean to true
		bRenderEnded = false;

		odprintf("THREAD [Render.RenderCPU()] Called & Running...");

		while (bRenderingThread)																	//This will create a infinite while loop until we toggle the boolean
		{
			this->bWFNFToggle = !this->bWFNFToggle;

			try
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				
				float fElapsedTime = elapsedTime.count();
				fElapsedTotal += fElapsedTime;
				if (fFps-fElapsedTime > 0 && fFps != 0) Sleep(fFps - fElapsedTime);

				if (screenSprite == nullptr || screenSpriteBuffer[0] == nullptr || screenSpriteBuffer[1] == nullptr )
				{
					odprintf("THREAD [Render.RenderCPU()] [%8.4f] ERROR: screenSprite == nullptr (possible exeptions to be thrown)!" );
					odprintf("THREAD [Render.RenderCPU()] [%8.4f] FIX:   recreated screenSprite into area.");

					screenSpriteBuffer[0] = new olcSprite(screenWidth, screenHeight);
					screenSpriteBuffer[1] = new olcSprite(screenWidth, screenHeight);

					screenSprite = screenSpriteBuffer[0];

				}

				//Swap the buffers and set the current buffer
				std::swap(screenSpriteBuffer[0], screenSpriteBuffer[1]);
				screenSprite = screenSpriteBuffer[1];

				for (int x = 0; x < screenSpriteBuffer[0]->nWidth; x++)
					for (int y = 0; y < screenSpriteBuffer[0]->nHeight; y++)
					{
						screenSpriteBuffer[0]->SetColour(x, y, ClearColor);
						screenSpriteBuffer[0]->SetGlyph(x, y, ClearChar);
					}

				//Update the map
				//map.Update();

				mat4x4 matWorld;
				matWorld = Matrix_MakeIdentity();	// Form World Matrix

				// Create "Point At" Matrix for camera
				vec3d vUp = { 0,1,0 };
				vec3d vTarget = { 0,0,1 };
				mat4x4 matCameraRot;
				mat4x4 matRotX = Matrix_MakeRotationX(fPitch);
				mat4x4 matRotY = Matrix_MakeRotationY(fYaw);
				matCameraRot = Matrix_MultiplyMatrix(matRotX, matRotY);
				vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
				vTarget = Vector_Add(map.vCamera, vLookDir);
				mat4x4 matCamera = Matrix_PointAt(map.vCamera, vTarget, vUp);

				// Make view matrix from camera
				mat4x4 matView = Matrix_QuickInverse(matCamera);

				// Store triagles for rastering later
				vector<triangle_roastered> vecTrianglesToRaster;

				float ftotalx = 0.0f, ftotaly = 0.0f, ftotalz = 0.0f;
				long ltotal = 0;

				// Draw Triangles
				for (int obsc = 0; obsc < map.objects.size(); obsc++)
					//for (triangle tri : map.objects[obsc]->object_mesh.tris)
					for (int triid = 0; triid < map.objects[obsc]->object_mesh.tris.size(); triid++)
					{
						triangle triTransformed, triViewed, tri = map.objects[obsc]->object_mesh.tris[triid];
						triangle_roastered triProjected;

						mat4x4 meshTrans;

						meshTrans = Matrix_MultiplyMatrix(map.objects[obsc]->position, map.objects[obsc]->rotation);							// Transform by translation

						tri.p[0] = Matrix_MultiplyVector(meshTrans, tri.p[0]);
						tri.p[1] = Matrix_MultiplyVector(meshTrans, tri.p[1]);
						tri.p[2] = Matrix_MultiplyVector(meshTrans, tri.p[2]);

						// World Matrix Transform
						triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
						triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
						triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
						triTransformed.t[0] = tri.t[0];
						triTransformed.t[1] = tri.t[1];
						triTransformed.t[2] = tri.t[2];

						float fNormal = Triangle_GetNormal(triTransformed, map.vCamera);

						// If ray is aligned with normal, then triangle is visible
						if (fNormal < 0.0f || !bRenderNormals)
						{
							// Convert World Space --> View Space
							triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
							triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
							triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
							triViewed.sym = triTransformed.sym;
							triViewed.col = triTransformed.col;
							triViewed.t[0] = triTransformed.t[0];
							triViewed.t[1] = triTransformed.t[1];
							triViewed.t[2] = triTransformed.t[2];

							// Clip Viewed Triangle against near plane, this could form two additional
							// additional triangles. 
							int nClippedTriangles = 0;
							triangle clipped[2];
							nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

							// We may end up with multiple triangles form the clip, so project as
							// required
							for (int n = 0; n < nClippedTriangles; n++)
							{
								// Project triangles from 3D --> 2D
								triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
								triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
								triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
								triProjected.col = clipped[n].col;
								triProjected.sym = clipped[n].sym;
								triProjected.t[0] = clipped[n].t[0];
								triProjected.t[1] = clipped[n].t[1];
								triProjected.t[2] = clipped[n].t[2];


								triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
								triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
								triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

								triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
								triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
								triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

								triProjected.t[0].w = 1.0f / triProjected.p[0].w;
								triProjected.t[1].w = 1.0f / triProjected.p[1].w;
								triProjected.t[2].w = 1.0f / triProjected.p[2].w;


								// Scale into view, we moved the normalising into cartesian space
								// out of the matrix.vector function from the previous videos, so
								// do this manually
								triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
								triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
								triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

								// X/Y are inverted so put them back
								triProjected.p[0].x *= -1.0f;
								triProjected.p[1].x *= -1.0f;
								triProjected.p[2].x *= -1.0f;
								triProjected.p[0].y *= -1.0f;
								triProjected.p[1].y *= -1.0f;
								triProjected.p[2].y *= -1.0f;

								// Offset verts into visible normalised space
								vec3d vOffsetView = { 1,1,0 };
								triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
								triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
								triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
								triProjected.p[0].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[0].y *= 0.5f * (float)this->screenHeight;
								triProjected.p[1].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[1].y *= 0.5f * (float)this->screenHeight;
								triProjected.p[2].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[2].y *= 0.5f * (float)this->screenHeight;

								triProjected.sprite = tri.spritePtr;;

								ftotalx += (triProjected.p[0].x + triProjected.p[1].x + triProjected.p[2].x) / 3;
								ftotaly += (triProjected.p[0].y + triProjected.p[1].y + triProjected.p[2].y) / 3;
								ftotalz += (triProjected.p[0].z + triProjected.p[1].z + triProjected.p[2].z) / 3;

								ltotal++;

								// Store triangle for sorting
								triProjected.triid = triid;
								triProjected.objid = obsc;

								vecTrianglesToRaster.push_back(triProjected);
							}
						}

					}

				ftotalx /= ltotal;
				ftotaly /= ltotal;
				ftotalz /= ltotal;

				// Sort triangles from back to front
				sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle_roastered& t1, triangle_roastered& t2)
				{
					float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
					float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
					return z1 > z2;
				});
				
				// Clear Depth Buffer
				for (int i = 0; i < this->screenWidth * this->screenHeight; i++)
					pDepthBuffer[i] = 0.0f;

				// Clear Depth Buffer
				//memset(pDepthBuffer, 0, sizeof(float) * (this->screenWidth * this->screenHeight));

				// Loop through all transformed, viewed, projected, and sorted triangles
				vector<triangle_roastered> triHigh;
				for (auto& triToRaster : vecTrianglesToRaster)
				{
					// Clip triangles against all four screen edges, this could yield
					// a bunch of triangles, so create a queue that we traverse to 
					//  ensure we only test new triangles generated against planes
					triangle clipped[2];
					list<triangle_roastered> listTriangles;

					// Add initial triangle
					listTriangles.push_back(triToRaster);
					int nNewTriangles = 1;

					for (int p = 0; p < 4; p++)
					{
						int nTrisToAdd = 0;
						while (nNewTriangles > 0)
						{
							// Take triangle from front of queue
							triangle_roastered test = listTriangles.front();
							listTriangles.pop_front();
							nNewTriangles--;

							// Clip it against a plane. We only need to test each 
							// subsequent plane, against subsequent new triangles
							// as all triangles after a plane clip are guaranteed
							// to lie on the inside of the plane. I like how this
							// comment is almost completely and utterly justified
							triangle rt = RoasteredToTri(test);
							switch (p)
							{
							case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, rt, clipped[0], clipped[1]); break;
							case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)this->screenHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, rt, clipped[0], clipped[1]); break;
							case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, rt, clipped[0], clipped[1]); break;
							case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)this->screenWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, rt, clipped[0], clipped[1]); break;
							}

							// Clipping may yield a variable number of triangles, so
							// add these new ones to the back of the queue for subsequent
							// clipping against next planes
							for (int w = 0; w < nTrisToAdd; w++)
							{
								triangle_roastered tta = TriToRoastered(clipped[w]);
								tta.sprite = triToRaster.sprite;
								tta.objid = triToRaster.objid;
								tta.triid = triToRaster.triid;

								listTriangles.push_back(tta);
							}
						}
						nNewTriangles = listTriangles.size();
					}

					// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
					for (auto& t : listTriangles)
					{
						if (bRenderSpecificObject)
							if (iRenderSpecificObject != t.triid) continue;

						SpriteTexturedTriangle(screenSpriteBuffer[0], pDepthBuffer, screenWidth,
							t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
							t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
							t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
							t.sprite);

						if(bHighlightObject && lHighlightObject == t.objid)
							SpriteFillTriangle(screenSpriteBuffer[0], t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_HALF, FG_WHITE | BG_BLACK);

						if (bHighlightTriangle && lHighlightTriangle == t.triid)
							triHigh.push_back(t);
						
					}
				}
				for (auto t : triHigh) SpriteDrawTriangle(screenSpriteBuffer[0], t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_HALF, FG_WHITE | BG_WHITE);

				if (floorf(fTimeElapsedFPS) < floorf(fElapsedTotal))
				{
					fTimeElapsedFPS = fElapsedTotal;
					lLastElapsedFPS = lElapsedFPS;
					lElapsedFPS = 0;
					odprintf("%ld | %f %f %f | %f %f", lLastElapsedFPS, map.vCamera.x, map.vCamera.y, map.vCamera.z, fPitch, fYaw);
				}
				else
				{
					lElapsedFPS++;
				}

			}
			catch (exception e)
			{
				odprintf("ERROR: %c",  e.what());
			}
		
		}

		bRenderEnded = true;

		return true;
	}

	bool WaitForNextFrame()
	{
		odprintf("Render[%p].WaitForNextFrame()...", this);

		debugTabIndex++;

		bool currentB = bWFNFToggle;

		while (bWFNFToggle == currentB)
		{
			odprintf("%d | %d", bWFNFToggle, currentB);
			Sleep(1);
		}

		debugTabIndex--;

		return true;
	}

	bool CopySprite(olcSprite* ret)
	{
		bPause = true;

		///olcSprite* ret = new olcSprite(screenSprite->nWidth, screenSprite->nHeight);

		for (int x = 0; x < ret->nWidth; x++)
			for (int y = 0; y < ret->nHeight; y++)
			{
				ret->SetColour(x, y, screenSprite->GetColour(x, y));
				ret->SetGlyph(x, y, screenSprite->GetGlyph(x, y));
			}

		bPause = false;

		return true;
	}

	olcSprite* GetSprite()
	{
		bPause = true;

		olcSprite* ret = new olcSprite(screenSprite->nWidth, screenSprite->nHeight);

		for (int x = 0; x < ret->nWidth; x++)
			for (int y = 0; y < ret->nHeight; y++)
			{
				ret->SetColour(x, y, screenSprite->GetColour(x,y));
				ret->SetGlyph(x, y, screenSprite->GetGlyph(x, y));
			}

		bPause = false;

		return ret;
	}

	void AddObject(object* o)
	{
		map.objects.push_back(o);
	}

	object* FindObject(wstring name)
	{
		for (auto o : map.objects)
		{
			if (wcscmp(o->object_mesh.name.c_str(), name.c_str()))
				return o;
		}

		return nullptr;
	}

	void SetObjects(vector<object*> objects)
	{
		this->map.objects = objects;
	}

};

#endif