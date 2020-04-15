#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <vector>

#include <locale>
#include <codecvt>

#include "olcConsoleGameEngine.h"
#include "objecth.h"
#include "tdmath.h"
#include "debug.h"
#include "string.h"
#include "float.h"

using namespace std;

enum ObjSpritePattern
{
	OSP_NULL = 0,
	OSP_REPEATED = 1,
};

struct objmaterial
{
	std::string name;
	olcSprite* sprite = nullptr;

	objmaterial() {}

	objmaterial(std::string file)
	{
		//Name the material
		this->name = file;

		//Convert string to wstring
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(file);

		//Load the sprite
		sprite = new olcSprite(wide);

	}
	
	objmaterial(std::string file, std::string name)
	{
		this->name = name;

		//Convert string to wstring
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(file);

		//Load the sprite
		sprite = new olcSprite(wide);

	}

	objmaterial(std::wstring file, std::string name)
	{
		this->name = name;

		sprite = new olcSprite(file);

	}

};

struct object
{
	mesh object_mesh;
	mesh* object_collision_mesh = nullptr;										//Mainly a pointer so we can reference the main mesh back
	ObjSpritePattern spritePattern;
	mat4x4 position, rotation;
	float rx, ry, rz;
	float lighting_range[2] = { 0.0f, 1.0f };
	bool frozen = true;															//For map data, if the object is frozen

	object()
	{
		odprintf("Created Object STRUCT!");
		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();
	}

	object(mesh m)
	{
		odprintf("Created Object STRUCT!");
		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();

		object_mesh = m;

	}

	object(string file, bool text)
	{
		odprintf("Created Object STRUCT [%s]!", file.c_str());

		object_mesh.LoadFromObjectFile(file, text);

		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();
	}

	object(string file, bool text, float x, float y, float z)
	{
		odprintf("Created Object STRUCT [%s, %f, %f, %f]!", file.c_str(), x, y, z);

		object_mesh.LoadFromObjectFile(file, text);

		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();

		setPosition(x, y, z);
	}

	bool clear()
	{
		object_mesh.tris.clear();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				position.m[i][j] = 1.0f;

		return true;

	}

	void fromMesh(mesh m)
	{
		object_mesh = m;
	}

	bool setPosition(float x, float y, float z)
	{
		position = Matrix_MakeTranslation(x, y, z);

		return true;
	}

	bool setPosition(vec3d p)
	{
		return setPosition(p.x, p.y, p.z);
	}

	bool setRotation(float x, float y, float z)
	{
		mat4x4  rotationx, rotationy, rotationz;

		rx = x;
		ry = y;
		rz = z;

		rotationx = Matrix_MakeRotationX(x);
		rotationy = Matrix_MakeRotationY(y);
		rotationz = Matrix_MakeRotationZ(z);

		rotation = Matrix_MakeIdentity();
		rotation = Matrix_MultiplyMatrix(rotationx, rotationy); // Transform by rotation
		rotation = Matrix_MultiplyMatrix(rotation, rotationz); // Transform by rotation

		return true;
	}
};

vector<object*> LoadFromObjectFile(string sFilename, olcSprite* sprite, bool bHasTexture = false)
{
	vector<object*> objects;
	mesh tempmesh;
	vector<triangle> tris;
	wstring name;
	string mtlFilename = sFilename;

	//Remove the .obj if needed
	const string ext = ".obj";
	if (sFilename != ext &&
		sFilename.size() > ext.size() &&
		sFilename.substr(sFilename.size() - ext.size()) == ".obj")
	{
		// if so then strip them off
		mtlFilename = sFilename.substr(0, sFilename.size() - ext.size());
	}
	mtlFilename += ".mtl";

	//Check if the file isnt already open
	ifstream fmtl(mtlFilename);
	bool bFoundMtlAvaliable = false;
	if (fmtl.is_open())
		bFoundMtlAvaliable = true;

	ifstream f(sFilename);
	if (!f.is_open())
		return vector<object*>();

	// Local cache of verts
	vector<vec3d> verts;
	vector<vec2d> texs;
	vector<objmaterial* > materials;
	objmaterial* currMaterial = new objmaterial();

	//Handle MTL File data
	string sline;
	int i = 0;
	if(bFoundMtlAvaliable)
	while (!fmtl.eof())
	{
		//First get the line
		std::getline(fmtl,sline);

		//Check if the command is to create a new material
		string substrtochecl = string("newmtl ");
		std::size_t foundpos = sline.find(substrtochecl);
		if (foundpos != std::string::npos && foundpos == 0)
		{
			i++;
			string mname = sline.substr(substrtochecl.length(), sline.length() - substrtochecl.length());
			materials.push_back(new objmaterial(L"resources\\default.spr", mname));
		}
		
		//Check if the command is to load a material
		substrtochecl = string("map_Kd ");
		foundpos = sline.find(substrtochecl);
		if (foundpos != std::string::npos && foundpos == 0)
		{
			string mname = sline.substr(substrtochecl.length(), sline.length() - substrtochecl.length());
			//materials.push_back(new objmaterial(mname, L"resources\\default.spr"));
			mname = "resources\\" + mname + ".spr";
			wstring wmname = to_wstring(mname);

			materials[materials.size() - 1]->sprite = new olcSprite(wmname);
			ifstream fmat(mname);
			if (!fmat.is_open())
			{
				odprintf("Warning: resource not found [%s]! Please make sure all of your resources are correct! Material [%i] has been defaulted!", mname.c_str(), i);
			}
		}


	}

	//Handle OBJ File data
	char line[512];
	i = 0;
	while (!f.eof())
	{
		f.getline(line, 512);

		strstream s;
		s << line;

		char junk;

		i++;
		
		if (line[0] == 'u' && line[3] == 'm' && line[5] == 'l')
		{
			for (int j = 0; j < materials.size(); j++)
			{
				if (materials[j]->name == string(line + 7))
				{
					currMaterial = materials[j];
				}
			}
		}

		if (line[0] == 'o')
		{
			if (objects.size() == 0 && name.length() == 0)
			{
				wstring stringline(&line[0], &line[128]);									//Get the current name of the object
				name = stringline.substr(2, stringline.length());

			}
			else
			{
			//	//convert all values into minifloat
			//	for (int i = 0; i < tris.size(); i++)
			//	{
			//		for (int j = 0; j < 3; j++)
			//		{
			//			odprintf("%f %f ", tris[i].p[j].x , float8_to_float(float_to_float8(tris[i].p[j].x)));

			//			//tris[i].p[j].x = float8_to_float(float_to_float8(tris[i].p[j].x));
			//		}
			//	}

				tempmesh = mesh{ tris, {vec3d(), vec3d()}, name };							//Set the mesh to 
				tempmesh.GetBounds();														//Get the map bounds
				objects.push_back(new object(tempmesh));									//Add the mesh to the list
				tris.clear();																//Clear the tirangle list

				wstring stringline(&line[0], &line[128]);									//Get the current name of the object
				name = stringline.substr(2, stringline.length());

			}

		}

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				vec2d v;
				s >> junk >> junk >> v.u >> v.v;
				// A little hack for the spyro texture
				// Another Note: Its a dam well good hack for anything else -_-
				//v.u = 1.0f - v.u;
				v.v = 1.0f - v.v;
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
				tris[tris.size() - 1].spritePtr = currMaterial->sprite;
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
				tris[tris.size() - 1].spritePtr = currMaterial->sprite;

			}

		}
	}

	//We need to add the ending object to the list
	tempmesh = mesh{ tris, {vec3d(), vec3d()}, name };							//Set the mesh to 
	tempmesh.GetBounds();														//Get the map bounds
	objects.push_back(new object(tempmesh));									//Add the mesh to the list


	return objects;
}

#endif