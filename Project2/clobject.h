#ifndef CLOBJECTH_H
#define CLOBJECTH_H

#include "objecth.h"
#include "opencl.h"

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>

#include <CL/cl.hpp>

// Created a 2D structure to hold texture coordinates
struct cl_vec2d
{
	cl_float u = 0;
	cl_float v = 0;
	cl_float w = 1;
};

struct cl_vec3d
{
	cl_float x = 0;
	cl_float y = 0;
	cl_float z = 0;
	cl_float w = 1; // Need a 4th term to perform sensible matrix vector multiplication
};

struct cl_triangle
{
	vec3d p[3];
	vec2d t[3]; // added a texture coord per vertex
	wchar_t sym;
	cl_short col;
	bool calculated = false;
};


struct cl_mat4x4
{
	cl_float m[4][4] = { 0 };
};

cl_mat4x4 clConvertMat4x4(mat4x4 mat)
{
	cl_mat4x4 r;

	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
			r.m[x][y] = (cl_float)mat.m[x][y];

	return r;
}

cl_vec3d clConvertVector(vec3d vec)
{
	return { (cl_float)vec.x, (cl_float)vec.y, (cl_float)vec.z };
}

#endif