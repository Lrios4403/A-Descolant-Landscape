#ifndef TD_MAP_H
#define TD_MAP_H

#include <iostream>
#include <vector>
#include <string>

#include "object.h"
#include "tdcollision.h"

struct TDMap
{
	std::vector<object*> objects;
	std::vector<vec3d*> lights;

	vec3d vCamera;							// Location of camera in world space
	vec3d vCameraLastSafe;

	vector<object*> playerHitbox;
	vector<object*> floorHitbox;
	vector<object*> wallHitbox;

	float playerHeight = 3.3f;

	int fMapDiv = 1;
	unsigned long lPointerMapSize = 0;
	float fmapminx = 0;
	float fmapminz = 0;
	float fmapmaxx = 0;
	float fmapmaxz = 0;
	unsigned long lmapwidth = 0;
	unsigned long lmapheight = 0;
	float* fFloor;
	bool* bWalls;

	TDMap() {};

	~TDMap() 
	{
		objects.clear();
		objects.shrink_to_fit();
		lights.clear();
		lights.shrink_to_fit();
		playerHitbox.clear();
		playerHitbox.shrink_to_fit();

	};

	int Initalize(std::string objpath)
	{
		odprintf("Initalizing TDMap instance [%p]...!\n", this);
		debugTabIndex++;

		odprintb("Loading object file...");
		objects = LoadFromObjectFile(objpath, new olcSprite());

		if (objects.size() > 0)
		{
			fmapminx = fmapmaxx = objects[0]->object_mesh.tris[0].p[0].x;
			fmapminz = fmapmaxz = objects[0]->object_mesh.tris[0].p[0].z;

			for (auto obj : objects)
				for (auto t : obj->object_mesh.tris)
				{
					float fix = fmin(fmin(fmin(t.p[0].x, t.p[1].x), fmin(t.p[0].x, t.p[2].x)), fmin(t.p[2].x, t.p[1].x));
					float fiz = fmin(fmin(fmin(t.p[0].z, t.p[1].z), fmin(t.p[0].z, t.p[2].z)), fmin(t.p[2].z, t.p[1].z));
					float fax = fmax(fmax(fmax(t.p[0].x, t.p[1].x), fmax(t.p[0].x, t.p[2].x)), fmax(t.p[2].x, t.p[1].x));
					float faz = fmax(fmax(fmax(t.p[0].z, t.p[1].z), fmax(t.p[0].z, t.p[2].z)), fmax(t.p[2].z, t.p[1].z));

					if (fmapminx > fix) fmapminx = fix;
					if (fmapmaxx < fax) fmapmaxx = fax;
					if (fmapminz > fiz) fmapminz = fiz;
					if (fmapmaxz < faz) fmapmaxz = faz;

				}

			lmapwidth = (ceil(fmapmaxx) - floor(fmapminx));
			lmapheight = (ceil(fmapmaxz) - floor(fmapminz));
			lPointerMapSize = lmapwidth * lmapheight;

			fFloor = (float*)malloc(sizeof(float) * lPointerMapSize * fMapDiv);
			bWalls = (bool*)malloc(sizeof(bool) * lPointerMapSize * fMapDiv);

			for (unsigned long l = 0; l < lPointerMapSize; l++) fFloor[l] = 0.0f;

			odprintf("Map array sizes %lu %lu %lu!", lPointerMapSize, lPointerMapSize*sizeof(float), lPointerMapSize*sizeof(bool));

		}

		odprintf("Initalized TDMap instance!");
		debugTabIndex--;
		return 0;
	}

	//void LoadFloors(string loc)
	//{
	//	vector<object*> objects = LoadFromObjectFile(loc, new olcSprite());

	//	//Load the values for the floor
	//	for (auto obj : objects)
	//		for (auto t : obj->object_mesh.tris)
	//		{
	//			float fminx = fmin(fmin(fmin(t.p[0].x, t.p[1].x), fmin(t.p[0].x, t.p[2].x)), fmin(t.p[2].x, t.p[1].x));
	//			float fminz = fmin(fmin(fmin(t.p[0].z, t.p[1].z), fmin(t.p[0].z, t.p[2].z)), fmin(t.p[2].z, t.p[1].z));
	//			float fmaxx = fmax(fmax(fmax(t.p[0].x, t.p[1].x), fmax(t.p[0].x, t.p[2].x)), fmax(t.p[2].x, t.p[1].x));
	//			float fmaxy = fmax(fmax(fmax(t.p[0].y, t.p[1].y), fmax(t.p[0].y, t.p[2].y)), fmax(t.p[2].y, t.p[1].y));
	//			float fmaxz = fmax(fmax(fmax(t.p[0].z, t.p[1].z), fmax(t.p[0].z, t.p[2].z)), fmax(t.p[2].z, t.p[1].z));

	//			for (float z = 0; z < fabs(fmaxz - fminz); z++)
	//				for (float x = 0; x < fabs(fmaxx - fminx); x++)
	//				{
	//					unsigned long px = (unsigned long)(fminx + x + fmapminx);
	//					unsigned long py = (unsigned long)((fminz + z + fmapminz) * lmapwidth);
	//					unsigned long index = px + py;

	//					if (fFloor[index] < fmaxy) fFloor[index] = fmaxy;
	//				}
	//		}

	//}

	//void LoadWalls(string loc)
	//{
	//	vector<object*> objects = LoadFromObjectFile(loc, new olcSprite());

	//	//Load the values for the floor
	//	for (auto obj : objects)
	//		for (auto t : obj->object_mesh.tris)
	//		{
	//			float fminx = fmin(fmin(fmin(t.p[0].x, t.p[1].x), fmin(t.p[0].x, t.p[2].x)), fmin(t.p[2].x, t.p[1].x));
	//			float fminz = fmin(fmin(fmin(t.p[0].z, t.p[1].z), fmin(t.p[0].z, t.p[2].z)), fmin(t.p[2].z, t.p[1].z));
	//			float fmaxx = fmax(fmax(fmax(t.p[0].x, t.p[1].x), fmax(t.p[0].x, t.p[2].x)), fmax(t.p[2].x, t.p[1].x));
	//			float fmaxz = fmax(fmax(fmax(t.p[0].z, t.p[1].z), fmax(t.p[0].z, t.p[2].z)), fmax(t.p[2].z, t.p[1].z));

	//			for (float z = 0; z < fabs(fmaxz - fminz); z++)
	//				for (float x = 0; x < fabs(fmaxx - fminx); x++)
	//				{
	//					unsigned long px = (unsigned long)(fminx + x - fmapminx);
	//					unsigned long py = (unsigned long)((fminz + z - fmapminz) * lmapwidth);
	//					unsigned long index = px + py;

	//					bWalls[index] = true;
	//				}
	//		}
	//}

	//int Update()
	//{
	//	unsigned long luCamIndex = floor((vCamera.z- fmapminz) * lmapwidth + (vCamera.x-fmapminx));
	//	vCamera.y = fFloor[luCamIndex];
	//	return 0;
	//}

};

#endif