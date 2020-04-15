#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <Windows.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4996)
#endif

int debugTabIndex = 0;

void __cdecl odprintf(const char* format, ...)
{
	char    buf[4096], * p = buf;
	va_list args;
	int     n;

	va_start(args, format);
	n = _vsnprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
	va_end(args);

	p += (n < 0) ? sizeof buf - 3 : n;

	while (p > buf && isspace(p[-1]))
		* --p = '\0';

	*p++ = '\r';
	*p++ = '\n';
	*p = '\0';

	wchar_t wtext[4096];
	mbstowcs(wtext+debugTabIndex, buf, strlen(buf) + 1);//Plus null
	LPWSTR ptr = wtext;

	for (int i = 0; i < debugTabIndex; i++)
		wtext[i] = 0x09;

	OutputDebugString(ptr);

}

#define odprintb(x) odprintf("%s", x ? "true" : "false");

#endif