#ifndef COLLISION_H
#define COLLISION_H

#define SMALL_NUM   0.00000000															// anything that avoids division overflow

#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

#include "objecth.h"

#define PI (float)22/7
#define TWO_PI PI*2.0f
#define HALF_PI PI/2.0f

#include "object.h"

struct collision
{
	bool plane = false;
	triangle tris;
	vec3d points;
};

vector<collision> CollisionRay(vec3d ray, vec3d rayend, object* o);
vector<collision> CollisionRay(vec3d ray, vec3d rayend, mesh* m);

vector<collision> CollisionRay(vec3d ray, vec3d rayend, object* o)						//There may be more than one intersected triangle
{

	return CollisionRay(ray, rayend, &o->object_mesh);
}

vector<collision> CollisionRay(vec3d ray, vec3d rayend, mesh* m)						//There may be more than one intersected triangle
{
	vector<collision> ret;																// The return varaible
	vec3d raydir = Vector_Normalise(rayend - ray);										// The ray direction
	vec3d u, v, n, I;																	// The triangle vectors
	vec3d dir, w0, w;
	float r, a, b;
	float uu, uv, vv, wu, wv, D;

	for (auto tri : m->tris)
	{
		u = Vector_Sub(tri.p[1], tri.p[0]);
		v = Vector_Sub(tri.p[2], tri.p[0]);
		n = Vector_CrossProduct(u, v);													// cross product

		if (n == vec3d())																// triangle is degenerate
			continue;																	// do not deal with this case

		dir = raydir - ray;
		w0 = raydir - tri.p[0];
		a = -Float_CrossProduct(n, w0);
		b = -Float_CrossProduct(n, dir);

		if (fabs(b) < SMALL_NUM)
		{																				// ray is  parallel to triangle plane
			if (a == 0)																	// ray lies in triangle plane
				ret.push_back({ true, tri , vec3d() });
			else continue;																// If it doesent continue
		}

		I = (ray + r) * dir;															// intersect point of ray and plane

		uu = Float_CrossProduct(u, u);
		uv = Float_CrossProduct(u, v);
		vv = Float_CrossProduct(v, v);
		w = I - tri.p[0];
		wu = Float_CrossProduct(w, u);
		wv = Float_CrossProduct(w, v);
		D = uv * uv - uu * vv;

		// get and test parametric coords
		float s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)															// I is outside T
		{
			ret.push_back({ false, tri, I });
		}

		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)													// I is outside T
		{
			ret.push_back({ false, tri, I });
		}

	}

	return ret;
}

vector<collision> CollisionSegment(segment s, object* o)
{
	vector<collision> ret;																// The return varaible

	for (auto tri : o->object_mesh.tris)
	{
		vec3d p = s.p0;
		vec3d q = s.p1;
		vec3d a = tri.p[0];
		vec3d b = tri.p[1];
		vec3d c = tri.p[2];

		vec3d pq = q - p;
		vec3d pa = a - p;
		vec3d pb = b - p;
		vec3d pc = c - p;

		float u = Vector_ScalarProduct(pq, pc, pb);
		if (u < SMALL_NUM)	continue;
		float v = Vector_ScalarProduct(pq, pa, pc);
		if (v < SMALL_NUM)	continue;
		float w = Vector_ScalarProduct(pq, pb, pa);
		if (w < SMALL_NUM)	continue;

		float denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom;

		ret.push_back(collision{ false, tri, {u,v,w} });
	}

	return ret;
}

#endif