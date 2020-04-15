/*===DEFAULT STRUCTURE DEFINES ===============================================================================================*/
#define DEF_VEC2D	{0.0f, 0.0f, 1.0f}
#define DEF_VEC3D	{0.0f, 0.0f, 0.0f, 1.0f}
#define DEF_TRIANGLE {{DEF_VEC3D,DEF_VEC3D,DEF_VEC3D},{DEF_VEC2D,DEF_VEC2D,DEF_VEC2D}, 0, 0, false, 0}

/*===STRUCTURE DEFINES =======================================================================================================*/
typedef unsigned short wchar_t;
typedef wchar_t WCHAR;
typedef char CHAR;
typedef unsigned short WORD;

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};


enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

typedef struct _CHAR_INFO 
{
	union 
	{
		WCHAR UnicodeChar;
		CHAR   AsciiChar;
	} Char;
	WORD Attributes;
} CHAR_INFO, *PCHAR_INFO;

typedef struct tag_vec2d
{
	float u;
	float v;
	float w;

}vec2d;

typedef struct tag_vec3d
{
	float x;
	float y;
	float z;
	float w; // Need a 4th term to perform sensible matrix vector multiplication

}vec3d;

typedef struct tag_mat4x4
{
	float m[4][4];
}mat4x4;

typedef struct tag_triangle
{
	struct tag_vec3d p[3];
	struct tag_vec2d t[3]; // added a texture coord per vertex
	int triid;
	wchar_t sym;
	short col;
	bool calculated;
	void* spritePtr;

}triangle;

/*===MATH PREIDENTIFIERS======================================================================================================*/
float Triangle_ClipAgainstPlane_Dist(vec3d plane_n, vec3d plane_p, vec3d p);
float Vector_IntersectPlane_t(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd);
void MultiplyMatrixTriangle(triangle* it, triangle* ot, const mat4x4* m);
void MultiplyMatrixVector(vec3d* i, vec3d* o, mat4x4* m);
vec3d Matrix_MultiplyVector(mat4x4 *m, vec3d i);
mat4x4 Matrix_MakeIdentity();
mat4x4 Matrix_MakeRotationX(float fAngleRad);
mat4x4 Matrix_MakeRotationY(float fAngleRad);
mat4x4 Matrix_MakeRotationZ(float fAngleRad);
mat4x4 Matrix_MakeTranslation(float x, float y, float z);
mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
mat4x4 Matrix_MultiplyMatrix(mat4x4 *m1, mat4x4 *m2);
mat4x4 Matrix_PointAt(vec3d pos, vec3d target, vec3d up);
mat4x4 Matrix_QuickInverse(mat4x4 *m);
vec3d Vector_Add(vec3d v1, vec3d v2);
vec3d Vector_Sub(vec3d v1, vec3d v2);
vec3d Vector_Mul(vec3d v1, float k);
vec3d Vector_Div(vec3d v1, float k); 
float Vector_DotProduct(vec3d v1, vec3d v2);
float Vector_Length(vec3d v);
vec3d Vector_Normalise(vec3d v);
vec3d Vector_CrossProduct(vec3d v1, vec3d v2);
vec3d Vector_IntersectPlane(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd);
int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle *in_tri, triangle *out_tri1, triangle *out_tri2);
// Taken From Command Line Webcam Video
CHAR_INFO GetColour(float lum);

/*===MANUAL MATH ROUTINES=====================================================================================================*/
float Triangle_ClipAgainstPlane_Dist(vec3d plane_n, vec3d plane_p, vec3d p)
{
	// Return signed shortest distance from point to plane, plane normal must be normalised
	vec3d n = Vector_Normalise(p);
	return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
}

float Vector_IntersectPlane_t(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	return t;
}

void MultiplyMatrixTriangle(triangle* it, triangle* ot, const mat4x4* m)
{
	for(int i = 0; i < 3; i++)
	{
		ot->p[i].x = it->p[i].x * m->m[0][0] + it->p[i].y * m->m[1][0] + it->p[i].z * m->m[2][0] + m->m[3][0];
		ot->p[i].y = it->p[i].x * m->m[0][1] + it->p[i].y * m->m[1][1] + it->p[i].z * m->m[2][1] + m->m[3][1];
		ot->p[i].z = it->p[i].x * m->m[0][2] + it->p[i].y * m->m[1][2] + it->p[i].z * m->m[2][2] + m->m[3][2];
		float w = it->p[i].x * m->m[0][3] + it->p[i].y * m->m[1][3] + it->p[i].z * m->m[2][3] + m->m[3][3];
	
		if (w != 0.0f)
		{
			ot->p[i].x /= w; ot->p[i].y /= w; ot->p[i].z /= w;
		}
	}
}

/*===JAVIDX9 MATH ROUTINES====================================================================================================*/

void MultiplyMatrixVector(vec3d* i, vec3d* o, mat4x4* m)
{
	o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
	o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
	o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
	float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];

	if (w != 0.0f)
	{
		o->x /= w; o->y /= w; o->z /= w;
	}
}

vec3d Matrix_MultiplyVector(mat4x4 *m, vec3d i)
{
	vec3d v = DEF_VEC3D;
	v.x = i.x * m->m[0][0] + i.y * m->m[1][0] + i.z * m->m[2][0] + i.w * m->m[3][0];
	v.y = i.x * m->m[0][1] + i.y * m->m[1][1] + i.z * m->m[2][1] + i.w * m->m[3][1];
	v.z = i.x * m->m[0][2] + i.y * m->m[1][2] + i.z * m->m[2][2] + i.w * m->m[3][2];
	v.w = i.x * m->m[0][3] + i.y * m->m[1][3] + i.z * m->m[2][3] + i.w * m->m[3][3];
	return v;
}

mat4x4 Matrix_MakeIdentity()
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cos(fAngleRad);
	matrix.m[1][2] = sin(fAngleRad);
	matrix.m[2][1] = -sin(fAngleRad);
	matrix.m[2][2] = cos(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cos(fAngleRad);
	matrix.m[0][2] = sin(fAngleRad);
	matrix.m[2][0] = -sin(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cos(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cos(fAngleRad);
	matrix.m[0][1] = sin(fAngleRad);
	matrix.m[1][0] = -sin(fAngleRad);
	matrix.m[1][1] = cos(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tan(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4 *m1, mat4x4 *m2)
{
	mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1->m[r][0] * m2->m[0][c] + m1->m[r][1] * m2->m[1][c] + m1->m[r][2] * m2->m[2][c] + m1->m[r][3] * m2->m[3][c];
	return matrix;
}

mat4x4 Matrix_PointAt(vec3d pos, vec3d target, vec3d up)
{
	// Calculate new forward direction
	vec3d newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalise(newForward);

	// Calculate new Up direction
	vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	vec3d newUp = Vector_Sub(up, a);
	newUp = Vector_Normalise(newUp);

	// New Right direction is easy, its just cross product
	vec3d newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

mat4x4 Matrix_QuickInverse(mat4x4 *m) // Only for Rotation/Translation Matrices
{
	mat4x4 matrix;
	matrix.m[0][0] = m->m[0][0]; matrix.m[0][1] = m->m[1][0]; matrix.m[0][2] = m->m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m->m[0][1]; matrix.m[1][1] = m->m[1][1]; matrix.m[1][2] = m->m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m->m[0][2]; matrix.m[2][1] = m->m[1][2]; matrix.m[2][2] = m->m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m->m[3][0] * matrix.m[0][0] + m->m[3][1] * matrix.m[1][0] + m->m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m->m[3][0] * matrix.m[0][1] + m->m[3][1] * matrix.m[1][1] + m->m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m->m[3][0] * matrix.m[0][2] + m->m[3][1] * matrix.m[1][2] + m->m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

vec3d Vector_Add(vec3d v1, vec3d v2)
{
	vec3d r;
	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;
	r.z = v1.z + v2.z;
	return r;
}

vec3d Vector_Sub(vec3d v1, vec3d v2)
{
	vec3d r;
	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;
	r.z = v1.z - v2.z;
	return r;
}

vec3d Vector_Mul(vec3d v1, float k)
{
	vec3d r;
	r.x = v1.x * k;
	r.y = v1.y * k;
	r.z = v1.z * k;
	return r;
}

vec3d Vector_Div(vec3d v1, float k)
{
	vec3d r;
	r.x = v1.x / k;
	r.y = v1.y / k;
	r.z = v1.z / k;
	return r;
}

float Vector_DotProduct(vec3d v1, vec3d v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

float Vector_Length(vec3d v)
{
	return sqrt(Vector_DotProduct(v, v));
}

vec3d Vector_Normalise(vec3d v)
{
	float l = Vector_Length(v);
	vec3d r;
	r.x = v.x / l;
	r.y = v.y / l;
	r.z = v.z / l;
	return r;
}

vec3d Vector_CrossProduct(vec3d v1, vec3d v2)
{
	vec3d v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

vec3d Vector_IntersectPlane(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	vec3d lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}

int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle *in_tri, triangle *out_tri1, triangle *out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = Vector_Normalise(plane_n);

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d inside_points[3];  int nInsidePointCount = 0;
	vec3d outside_points[3]; int nOutsidePointCount = 0;
	vec2d inside_tex[3]; int nInsideTexCount = 0;
	vec2d outside_tex[3]; int nOutsideTexCount = 0;
	
	/*	
	printf("[#] FUNC TRICLIPPED PLANE_P       %f %f %f \n", plane_p.x, plane_p.y, plane_p.z);
	printf("[#] FUNC TRICLIPPED PLANE_N       %f %f %f \n", plane_n.x, plane_n.y, plane_n.z);

	printf("\n");

	// Get signed distance of each point in triangle to plane
	*//*
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);
	*/
	
	float d0 = Triangle_ClipAgainstPlane_Dist(plane_n, plane_p, in_tri->p[0]);
	float d1 = Triangle_ClipAgainstPlane_Dist(plane_n, plane_p, in_tri->p[1]);
	float d2 = Triangle_ClipAgainstPlane_Dist(plane_n, plane_p, in_tri->p[2]);
	
	if (d0 >= 0) { inside_points[nInsidePointCount++] = in_tri->p[0]; inside_tex[nInsideTexCount++] = in_tri->t[0]; }
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[0]; outside_tex[nOutsideTexCount++] = in_tri->t[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = in_tri->p[1]; inside_tex[nInsideTexCount++] = in_tri->t[1];
	}
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[1];  outside_tex[nOutsideTexCount++] = in_tri->t[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = in_tri->p[2]; inside_tex[nInsideTexCount++] = in_tri->t[2];
	}
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[2];  outside_tex[nOutsideTexCount++] = in_tri->t[2];
	}

/*
	for(int i = 0; i < 3; i++)
	printf("[#] FUNC TRICLIPPED POINTS IN     %f %f %f \n", inside_points[i].x, inside_points[i].y, inside_points[i].z);
	for(int i = 0; i < 3; i++)
	printf("[#] FUNC TRICLIPPED POINTS OUT    %f %f %f \n", outside_points[i].x, outside_points[i].y, outside_points[i].z);
	printf("\n");
	for(int i = 0; i < 3; i++)
	printf("[#] FUNC TRICLIPPED TEXTS IN      %f %f %f \n", inside_tex[i].u, inside_tex[i].v, inside_tex[i].w);
	for(int i = 0; i < 3; i++)
	printf("[#] FUNC TRICLIPPED TEXTS OUT     %f %f %f \n", outside_tex[i].u, outside_tex[i].v, outside_tex[i].w);
	printf("\n");
	
	printf("[#] FUNC TRICLIPPED COUNTER	%i %i %i %i \n", nInsidePointCount, nOutsidePointCount, nInsideTexCount, nOutsideTexCount);

	printf("\n");
*/
	
	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1[0] = in_tri[0];

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1->col = in_tri->col;
		out_tri1->sym = in_tri->sym;

		// The inside point is valid, so keep that...
		out_tri1->p[0] = inside_points[0];
		out_tri1->t[0] = inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t = Vector_IntersectPlane_t(plane_p, plane_n, inside_points[0], outside_points[0]);
		out_tri1->p[1] = Vector_IntersectPlane(plane_p, plane_n, inside_points[0], outside_points[0]);
		out_tri1->t[1].u = t * (outside_tex[0].u - inside_tex[0].u) + inside_tex[0].u;
		out_tri1->t[1].v = t * (outside_tex[0].v - inside_tex[0].v) + inside_tex[0].v;
		out_tri1->t[1].w = t * (outside_tex[0].w - inside_tex[0].w) + inside_tex[0].w;

		t = Vector_IntersectPlane_t(plane_p, plane_n, inside_points[0], outside_points[1]);
		out_tri1->p[2] = Vector_IntersectPlane(plane_p, plane_n, inside_points[0], outside_points[1]);
		out_tri1->t[2].u = t * (outside_tex[1].u - inside_tex[0].u) + inside_tex[0].u;
		out_tri1->t[2].v = t * (outside_tex[1].v - inside_tex[0].v) + inside_tex[0].v;
		out_tri1->t[2].w = t * (outside_tex[1].w - inside_tex[0].w) + inside_tex[0].w;

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1->col = in_tri->col;
		out_tri1->sym = in_tri->sym;

		out_tri2->col = in_tri->col;
		out_tri2->sym = in_tri->sym;

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1->p[0] = inside_points[0];
		out_tri1->p[1] = inside_points[1];
		out_tri1->t[0] = inside_tex[0];
		out_tri1->t[1] = inside_tex[1];

		float t = Vector_IntersectPlane_t(plane_p, plane_n, inside_points[0], outside_points[0]);
		out_tri1->p[2] = Vector_IntersectPlane(plane_p, plane_n, inside_points[0], outside_points[0]);
		out_tri1->t[2].u = t * (outside_tex[0].u - inside_tex[0].u) + inside_tex[0].u;
		out_tri1->t[2].v = t * (outside_tex[0].v - inside_tex[0].v) + inside_tex[0].v;
		out_tri1->t[2].w = t * (outside_tex[0].w - inside_tex[0].w) + inside_tex[0].w;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2->p[0] = inside_points[1];
		out_tri2->t[0] = inside_tex[1];
		out_tri2->p[1] = out_tri1->p[2];
		out_tri2->t[1] = out_tri1->t[2];
		t = Vector_IntersectPlane_t(plane_p, plane_n, inside_points[1], outside_points[0]);
		out_tri2->p[2] = Vector_IntersectPlane(plane_p, plane_n, inside_points[1], outside_points[0]);
		out_tri2->t[2].u = t * (outside_tex[0].u - inside_tex[1].u) + inside_tex[1].u;
		out_tri2->t[2].v = t * (outside_tex[0].v - inside_tex[1].v) + inside_tex[1].v;
		out_tri2->t[2].w = t * (outside_tex[0].w - inside_tex[1].w) + inside_tex[1].w;
		return 2; // Return two newly formed triangles which form a quad
	}
}

// Taken From Command Line Webcam Video
CHAR_INFO GetColour(float lum)
{
	short bg_col, fg_col;
	wchar_t sym;
	int pixel_bw = (int)(13.0f * lum);
	switch (pixel_bw)
	{
	case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
	case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
	default:
		bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO c;
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}

/*===MAIN 3D GRAPHICAL FUNCTIONS==============================================================================================*/

__kernel void RasterTriangle
	(
	__global triangle* tris,  __global const mat4x4* mz,  __global const mat4x4* mx, 
	__global const vec3d* vcam, __global const int* swidth, __global const int* sheight, __global const mat4x4* mp,
	__global const vec3d* vlight, __global const mat4x4* mw,  __global const mat4x4* mv, 
	__global const int* ots, __global triangle* outtris
	)
{	
	int gid = get_global_id(0); // = gid;
	int triangleOutputSize = ots[0];
	triangle triProjected = DEF_TRIANGLE, triTransformed = DEF_TRIANGLE, triViewed = DEF_TRIANGLE;
	triangle tri = tris[gid];
	mat4x4 matRotZ = mz[0];
	mat4x4 matRotX = mx[0];
	mat4x4 matProj = mp[0];
	mat4x4 matWorld = mw[0];
	mat4x4 matView = mv[0];
	vec3d vCamera = vcam[0];
	vec3d light_direction = vlight[0];
	float ScreenWidth = (float) swidth[0];
	float ScreenHeight = (float) sheight[0];
	
	//Print the entry
//	printf("RasterTriangle[%i] Called %f %f \n", gid, ScreenWidth, ScreenHeight);
//	printf("\n");
	
	//Print the current triangle
//	for(int i = 0; i < 3; i++)
//		printf("[%i] TRI         %f %f %f %f \n", gid, tri.p[i].x, tri.p[i].y, tri.p[i].z, tri.p[i].w);
	
//	printf("\n");
	
	//Print the matWorld jic
//	for(int x = 0; x < 4; x++)
//		printf("[%i] MATWORLD    %f %f %f %f \n", gid, matWorld.m[x][0], matWorld.m[x][1], matWorld.m[x][2], matWorld.m[x][3]);
	
//	printf("\n");
	
	//World Matrix Transform
	triTransformed.p[0] = Matrix_MultiplyVector(&matWorld, tri.p[0]);
	triTransformed.p[1] = Matrix_MultiplyVector(&matWorld, tri.p[1]);
	triTransformed.p[2] = Matrix_MultiplyVector(&matWorld, tri.p[2]);
	triTransformed.t[0] = tri.t[0];
	triTransformed.t[1] = tri.t[1];
	triTransformed.t[2] = tri.t[2];
	
	//Print the current triangle
//	for(int i = 0; i < 3; i++)
//		printf("[%i] TRITRANS    %f %f %f %f \n", gid, triTransformed.p[i].x, triTransformed.p[i].y, triTransformed.p[i].z, triTransformed.p[i].w);
	
	
//	printf("\n");
	
	// Calculate triangle Normal
	vec3d normal = DEF_VEC3D, line1 = DEF_VEC3D, line2 = DEF_VEC3D;

	// Get lines either side of triangle
	line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
	line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

	// Take cross product of lines to get normal to triangle surface
	normal = Vector_CrossProduct(line1, line2);

	// You normally need to normalise a normal!
	normal = Vector_Normalise(normal);

	// Get Ray from triangle to camera
	vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);
	
	/*
	printf("[%i] VNORMAL     %f %f %f \n", gid, normal.x, normal.y, normal.z);
	printf("[%i] VLINE1      %f %f %f \n", gid, line1.x, line1.y, line1.z);
	printf("[%i] VLINE2      %f %f %f \n", gid, line2.x, line2.y, line2.z);
	printf("[%i] VCAMERARAY  %f %f %f \n", gid, vCameraRay.x, vCameraRay.y, vCameraRay.z);
	
	printf("\n");
	*/

	if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
	{
		//printf("TRUE \n");
		
		//Get the lighting
//		printf("[%i] VLIGHT      %f %f %f \n", gid, light_direction.x, light_direction.y, light_direction.z);
		light_direction = Vector_Normalise(light_direction);
		float dp = max(0.1f, Vector_DotProduct(light_direction, normal));
//		printf("[%i] VLIGHT      %f %f %f \n", gid, light_direction.x, light_direction.y, light_direction.z);
//		printf("[%i] FLOATDP     %f \n", gid, dp);
//		printf("\n");
		CHAR_INFO c = GetColour(dp);
		triTransformed.col = c.Attributes;
		triTransformed.sym = c.Char.UnicodeChar;
		
		// Convert World Space --> View Space
		triViewed.p[0] = Matrix_MultiplyVector(&matView, triTransformed.p[0]);
		triViewed.p[1] = Matrix_MultiplyVector(&matView, triTransformed.p[1]);
		triViewed.p[2] = Matrix_MultiplyVector(&matView, triTransformed.p[2]);
		triViewed.sym = triTransformed.sym;
		triViewed.col = triTransformed.col;
		triViewed.t[0] = triTransformed.t[0];
		triViewed.t[1] = triTransformed.t[1];
		triViewed.t[2] = triTransformed.t[2];
		
		//Print the current triangle
//		for(int i = 0; i < 3; i++)
//			printf("[%i] TRIVIEWED   %f %f %f %f \n", gid, triViewed.p[i].x, triViewed.p[i].y, triViewed.p[i].z, triViewed.p[i].w);
		
//		printf("\n");

		// Clip Viewed Triangle against near plane, this could form two additional
		// additional triangles. 
		int nClippedTriangles = 0;
		triangle clipped[2] = {DEF_TRIANGLE,DEF_TRIANGLE};
		vec3d vBounds1 = { 0.0f, 0.0f, 0.1f };
		vec3d vBounds2 = { 0.0f, 0.0f, 1.0f };
		
		nClippedTriangles = Triangle_ClipAgainstPlane(vBounds1, vBounds2, &triViewed, &clipped[0], &clipped[1]);
		
/*		//Print the clipped triangles for debugging
		printf("[%i] CLIPPED NO. %i \n", gid,nClippedTriangles); 
		for(int i = 0; i < 3; i++)
			printf("[%i] TRIVIEWED   %f %f %f %f \n", gid, triViewed.p[i].x, triViewed.p[i].y, triViewed.p[i].z, triViewed.p[i].w);
			
		for(int i = 0; i < 3; i++)
			printf("[%i] TRICLIPPED1 %f %f %f %f \n", gid, clipped[0].p[i].x, clipped[0].p[i].y, clipped[0].p[i].z, clipped[0].p[i].w);
			
		for(int i = 0; i < 3; i++)
			printf("[%i] TRICLIPPED2 %f %f %f %f \n", gid, clipped[1].p[i].x, clipped[1].p[i].y, clipped[1].p[i].z, clipped[1].p[i].w);
*/
			
		// We may end up with multiple triangles form the clip, so project as
		// required
		for (int n = 0; n < nClippedTriangles; n++)
		{
			// Project triangles from 3D --> 2D
			triProjected.p[0] = Matrix_MultiplyVector(&matProj, clipped[n].p[0]);
			triProjected.p[1] = Matrix_MultiplyVector(&matProj, clipped[n].p[1]);
			triProjected.p[2] = Matrix_MultiplyVector(&matProj, clipped[n].p[2]);
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
			triProjected.p[0].x *= 0.5f * ScreenWidth;
			triProjected.p[0].y *= 0.5f * ScreenHeight;
			triProjected.p[1].x *= 0.5f * ScreenWidth;
			triProjected.p[1].y *= 0.5f * ScreenHeight;
			triProjected.p[2].x *= 0.5f * ScreenWidth;
			triProjected.p[2].y *= 0.5f * ScreenHeight;
			triProjected.calculated = true;
		
			if(n==0) outtris[gid] = triProjected;
			else outtris[triangleOutputSize - gid] = triProjected;
			
		}
	}
}