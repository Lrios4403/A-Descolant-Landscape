#ifndef OLCSPRITE_H
#define OLCSPRITE_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "olcConsoleGameEngine.h"
#include "object.h"
#include "tdmath.h"
#include "debug.h"

using namespace std;

//Function predefines
void SpriteDraw(olcSprite* origin, int x, int y, short, short col);
void SpriteFill(olcSprite* origin, int x1, int y1, int x2, int y2, short c, short col);
void SpriteDrawString(olcSprite* origin, int x, int y, std::wstring c, short col);
void SpriteDrawStringAlpha(olcSprite* origin, int x, int y, std::wstring c, short col);
void SpriteClip(olcSprite* origin, int& x, int& y);
void SpriteDrawLine(olcSprite* origin, int x1, int y1, int x2, int y2, short c, short col);
void SpriteDrawTriangle(olcSprite* origin, int x1, int y1, int x2, int y2, int x3, int y3, short c, short col);
void SpriteFillTriangle(olcSprite* origin, int x1, int y1, int x2, int y2, int x3, int y3, short c, short col);
void SpriteDrawCircle(olcSprite* origin, int xc, int yc, int r, short c, short col);
void SpriteDrawPartialSprite(olcSprite* origin, int x, int y, olcSprite* sprite, int ox, int oy, int w, int h);
void SpriteTexturedTriangle(olcSprite* origin, float* pDepthBuffer, int screenWidth, int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	olcSprite* tex);

void SpriteDraw(olcSprite* origin, int x, int y, short c = 0x2588, short col = 0x000F)
{
	if (x >= 0 && x < origin->nWidth && y >= 0 && y < origin->nHeight)
	{
		origin->SetColour(x, y, col);
		origin->SetGlyph(x, y, c);
	}
}

void SpriteFill(olcSprite* origin, int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
{
	SpriteClip(origin, x1, y1);
	SpriteClip(origin, x2, y2);
	for (int x = x1; x < x2; x++)
		for (int y = y1; y < y2; y++)
			SpriteDraw(origin, x, y, c, col);
}

void SpriteDrawString(olcSprite* origin, int x, int y, std::wstring c, short col = 0x000F)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		origin->SetColour(x, y, col);
		origin->SetGlyph(x, y, c[i]);
	}
}

void SpriteDrawStringAlpha(olcSprite* origin, int x, int y, std::wstring c, short col = 0x000F)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		if (c[i] != L' ')
		{
			origin->SetColour(x, y, col);
			origin->SetGlyph(x, y, c[i]);
		}
	}
}

void SpriteClip(olcSprite* origin, int& x, int& y)
{
	if (x < 0) x = 0;
	if (x >= origin->nWidth) x = origin->nWidth;
	if (y < 0) y = 0;
	if (y >= origin->nHeight) y = origin->nHeight;
}

void SpriteDrawLine(olcSprite* origin, int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		SpriteDraw(origin, x, y, c, col);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			SpriteDraw(origin, x, y, c, col);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		SpriteDraw(origin, x, y, c, col);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			SpriteDraw(origin, x, y, c, col);
		}
	}
}

void SpriteDrawTriangle(olcSprite* origin, int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
{
	SpriteDrawLine(origin, x1, y1, x2, y2, c, col);
	SpriteDrawLine(origin, x2, y2, x3, y3, c, col);
	SpriteDrawLine(origin, x3, y3, x1, y1, c, col);
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void SpriteFillTriangle(olcSprite* origin, int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
{
	auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) SpriteDraw(origin, i, ny, c, col); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		SWAP(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // SpriteDraw line from min to max points found on the y
									 // Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

void SpriteDrawCircle(olcSprite* origin, int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
{
	int x = 0;
	int y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	while (y >= x) // only formulate 1/8 of circle
	{
		SpriteDraw(origin, xc - x, yc - y, c, col);//upper left left
		SpriteDraw(origin, xc - y, yc - x, c, col);//upper upper left
		SpriteDraw(origin, xc + y, yc - x, c, col);//upper upper right
		SpriteDraw(origin, xc + x, yc - y, c, col);//upper right right
		SpriteDraw(origin, xc - x, yc + y, c, col);//lower left left
		SpriteDraw(origin, xc - y, yc + x, c, col);//lower lower left
		SpriteDraw(origin, xc + y, yc + x, c, col);//lower lower right
		SpriteDraw(origin, xc + x, yc + y, c, col);//lower right right
		if (p < 0) p += 4 * x++ + 6;
		else p += 4 * (x++ - y--) + 10;
	}
}

void SpriteFillCircle(olcSprite* origin, int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
{
	// Taken from wikipedia
	int x = 0;
	int y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	auto drawline = [&](int sx, int ex, int ny)
	{
		for (int i = sx; i <= ex; i++)
			SpriteDraw(origin, i, ny, c, col);
	};

	while (y >= x)
	{
		// Modified to SpriteDraw scan-lines instead of edges
		drawline(xc - x, xc + x, yc - y);
		drawline(xc - y, xc + y, yc - x);
		drawline(xc - x, xc + x, yc + y);
		drawline(xc - y, xc + y, yc + x);
		if (p < 0) p += 4 * x++ + 6;
		else p += 4 * (x++ - y--) + 10;
	}
};

void SpriteDrawSprite(olcSprite* origin, int x, int y, olcSprite* sprite)
{
	if (sprite == nullptr)
		return;

	for (int i = 0; i < sprite->nWidth; i++)
	{
		for (int j = 0; j < sprite->nHeight; j++)
		{
			if (sprite->GetGlyph(i, j) != L' ')
				SpriteDraw(origin, x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
		}
	}
}

void SpriteDrawPartialSprite(olcSprite* origin, int x, int y, olcSprite* sprite, int ox, int oy, int w, int h)
{
	if (sprite == nullptr)
		return;

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (sprite->GetGlyph(i + ox, j + oy) != L' ')
				SpriteDraw(origin, x + i, y + j, sprite->GetGlyph(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
		}
	}
}

void SpriteTexturedTriangle(olcSprite* origin, float* pDepthBuffer, int screenWidth, 
	int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	olcSprite* tex)
{
	if (y2 < y1)
	{
		swap(y1, y2);
		swap(x1, x2);
		swap(u1, u2);
		swap(v1, v2);
		swap(w1, w2);
	}

	if (y3 < y1)
	{
		swap(y1, y3);
		swap(x1, x3);
		swap(u1, u3);
		swap(v1, v3);
		swap(w1, w3);
	}

	if (y3 < y2)
	{
		swap(y2, y3);
		swap(x2, x3);
		swap(u2, u3);
		swap(v2, v3);
		swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(ax, bx);
				swap(tex_su, tex_eu);
				swap(tex_sv, tex_ev);
				swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * screenWidth + j])
				{
					SpriteDraw(origin, j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * screenWidth + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(ax, bx);
				swap(tex_su, tex_eu);
				swap(tex_sv, tex_ev);
				swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * screenWidth + j])
				{
					SpriteDraw(origin, j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * screenWidth + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
}


#endif