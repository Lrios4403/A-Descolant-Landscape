#ifndef TD_PROCEDURAL_H
#define TD_PROCEDURAL_H

#include <iostream>
#include <vector>
#include <math.h>
#include <string>

#include "object.h"

object* objGenerateTerrainPerlinNoise(long trimapwidth, long trimaplength, float mapwidth, float maplength, float fmaxmapheight, int iOctives, float fBias)
{
	object* ret = new object();

	float triwidth = (mapwidth) / trimapwidth;
	float trilength = (maplength) / trimaplength;

	trimapwidth++;
	trimaplength++;

	vec3d* vv = new vec3d[trimapwidth * trimaplength];

	for (int x = 1; x < trimapwidth-1; x++)
		for (int y = 1; y < trimaplength-1; y++)
		{
			// Scale to seed range
			float fr = ((float)rand() / (float)RAND_MAX);
			vv[y * trimapwidth + x].y = fr*fmaxmapheight;

		}

	for (long z = 0; z < trimaplength; z++)
		for (long x = 0; x < trimapwidth; x++)
		{
			long vindex = (z * trimaplength) + x;

			vv[vindex].x = x * triwidth;
			vv[vindex].z = z * trilength;

		}

	for (long z = 0; z < trimaplength-1; z++)
		for (long x = 0; x < trimapwidth-1; x++)
		{
			vec3d v0 = vv[z * trimaplength + x];
			vec3d v1 = vv[z * trimaplength + x + 1];
			vec3d v2 = vv[(z + 1) * trimaplength + x];
			vec3d v3 = vv[(z + 1) * trimaplength + x + 1];

			triangle tri1, tri2;
			tri1.p[0] = v0; tri1.p[1] = v1; tri1.p[2] = v2;
			tri1.p[0] = v3; tri1.p[1] = v1; tri1.p[2] = v2;

			ret->object_mesh.tris.push_back(tri1);
			ret->object_mesh.tris.push_back(tri2);

		}


	return ret;
}

#endif