#ifndef OBJECTH_H
#define OBJECTH_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <codecvt>

#include "olcConsoleGameEngine.h"
#include "debug.h"

using namespace std;

// Created a 2D structure to hold texture coordinates
struct vec2d
{
	float u = 0;
	float v = 0;
	float w = 1;
};

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // Need a 4th term to perform sensible matrix vector multiplication

	bool operator==(vec3d a) const
	{
		if (a.x == x && a.y == y && a.z == z && a.w == w)
			return true;
		else
			return false;
	}

	vec3d operator+(const vec3d& a) const
	{
		return vec3d{ a.x + x, a.y + y, a.z + z, w };
	}

	vec3d operator-(const vec3d& a) const
	{
		return vec3d{ a.x - x, a.y - y, a.z - z, w };
	}

	vec3d operator*(const vec3d& a) const
	{
		return vec3d{ a.x * x, a.y * y, a.z * z, w };
	}

	vec3d operator/(const vec3d& a) const
	{
		return vec3d{ a.x / x, a.y / y, a.z / z, w };
	}

	vec3d operator+(const float& a) const
	{
		return vec3d{ a + x, a + y, a + z, w };
	}

	vec3d operator-(const float& a) const
	{
		return vec3d{ a - x, a - y, a - z, w };
	}

	vec3d operator*(const float& a) const
	{
		return vec3d{ a * x, a * y, a * z, w };
	}

	vec3d operator/(const float& a) const
	{
		return vec3d{ a / x, a / y, a / z, w };
	}
};

struct ray3d
{
	vec3d vstart, vend;
	float length;
	vec3d dir, lendir;

	ray3d(vec3d vs, vec3d ve)
	{
		vstart = vs;
		vend = ve;

		Configure();
	}

	bool Configure()
	{
		lendir = vend - vstart;
		length = sqrtf(lendir.x * lendir.x + lendir.y * lendir.y + lendir.z * lendir.z);
		dir.x = lendir.x / length;
		dir.y = lendir.y / length;
		dir.z = lendir.z / length;

		return true;
	}

};

struct segment
{
	vec3d p0, p1;

};

struct rectangle
{
	vec3d p0, p1;
	float bottomX, topX, bottomY, topY, bottomZ, topZ;

	void GetOrganizedFloats()
	{
		if (p0.x < p1.x)
		{
			bottomX = p0.x;
			topX = p1.x;
		}
		else
		{
			bottomX = p1.x;
			topX = p0.x;
		}

		if (p0.z < p1.z)
		{
			bottomZ = p0.z;
			topZ = p1.z;
		}
		else
		{
			bottomZ = p1.z;
			topZ = p0.z;
		}
	}

};

struct triangle
{
	vec3d p[3];
	vec2d t[3]; // added a texture coord per vertex
	int triid = 0;
	wchar_t sym;
	short col;
	bool calculated = false;
	olcSprite* spritePtr;

	void addOffset(vec3d v)
	{
		p[0].x += v.x; p[1].x += v.x; p[2].x += v.x;
		p[0].y += v.y; p[1].y += v.y; p[2].y += v.y;
		p[0].z += v.z; p[1].z += v.z; p[2].z += v.z;
	}
};

struct mesh
{
	vector<triangle> tris;
	vec3d vMapBounds[2];
	wstring name;

	bool LoadFromObjectFile(string sFilename, bool bHasTexture = false)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;
		vector<vec2d> texs;

		int i = 0;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			i++;

			if (line[0] == 'o')
			{
				name = wstring(&line[0], &line[128]);
			}

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					vec2d v;
					s >> junk >> junk >> v.u >> v.v;
					// A little hack for the spyro texture
					//v.u = 1.0f - v.u;
					//v.v = 1.0f - v.v;
					texs.push_back(v);
				}
				else
				{
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					if (strchr(line, '/') != NULL)
					{
						goto gotobHasTexture;
					}

					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
			gotobHasTexture:

				if (line[0] == 'f')
				{
					s >> junk;

					string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

				}

			}
		}

		GetBounds();

		return true;

	}

	bool GetBounds()
	{
		for (int i = 0; i < tris.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (tris[i].p[j].x > vMapBounds[0].x) vMapBounds[0].x = tris[i].p[j].x;
				if (tris[i].p[j].y > vMapBounds[0].y) vMapBounds[0].y = tris[i].p[j].y;
				if (tris[i].p[j].z > vMapBounds[0].z) vMapBounds[0].z = tris[i].p[j].z;

				if (tris[i].p[j].x < vMapBounds[1].x) vMapBounds[1].x = tris[i].p[j].x;
				if (tris[i].p[j].y < vMapBounds[1].y) vMapBounds[1].y = tris[i].p[j].y;
				if (tris[i].p[j].z < vMapBounds[1].z) vMapBounds[1].z = tris[i].p[j].z;

			}
		}

		return true;
	}

	bool Resize(float f)
	{
		for (int i = 0; i < tris.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				tris[i].p[j].x *= f;
				tris[i].p[j].y *= f;
				tris[i].p[j].z *= f;
			}
		}

		for (int i = 0; i < tris.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (tris[i].p[j].x < vMapBounds[0].x) vMapBounds[0].x = tris[i].p[j].x;
				if (tris[i].p[j].y < vMapBounds[0].y) vMapBounds[0].y = tris[i].p[j].y;
				if (tris[i].p[j].z < vMapBounds[0].z) vMapBounds[0].z = tris[i].p[j].z;

				if (tris[i].p[j].x > vMapBounds[1].x) vMapBounds[1].x = tris[i].p[j].x;
				if (tris[i].p[j].y > vMapBounds[1].y) vMapBounds[1].y = tris[i].p[j].y;
				if (tris[i].p[j].z > vMapBounds[1].z) vMapBounds[1].z = tris[i].p[j].z;

			}
		}

		return true;
	}

};

struct mat4x4
{
	float m[4][4] = { 0 };
};

struct plane3d
{
	vec3d p[4];
	vec3d normal;
	float d;

	void fromTriangle(triangle t)
	{
		vec3d a = t.p[0];
		vec3d b = t.p[1];
		vec3d c = t.p[2];

		vec3d ba = { a.x - b.x,a.y - b.y, a.z - b.z };
		vec3d bc = { c.x - b.x,c.y - b.y, c.z - b.z };

		normal.x = bc.y * ba.z - bc.z * ba.y;
		normal.y = bc.z * ba.x - bc.x * ba.z;
		normal.z = bc.x * ba.y - bc.y * ba.x;

		if (normal.x == normal.y == normal.z == 0)
		{
			d = 0.0f;
			return;
		}

		float l = std::sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal = vec3d{ normal.x / l, normal.y / l, normal.z / l , 1.0f};

		d = -(normal.x * a.x + normal.y * a.y + normal.z * a.z);

	}

	inline float distanceToPoint(vec3d v)
	{
		return (normal.x * v.x + normal.y * v.y + normal.z * v.z)+d;
	}

	bool isDegenerate()
	{
		return (normal.x == 0.0f) && (normal.y == 0.0f) && (normal.z == 0.0f);
	}
};

#endif