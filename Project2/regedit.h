#ifndef REGEDIT_H
#define REGEDIT_H

#include <Windows.h>
#include <iostream>

/*===================================================================================================================================================*/
/*
/*	A literal copypasta from: https://www.youtube.com/watch?v=MdSIr_z9NLM
/*
/*===================================================================================================================================================*/

bool bRegeditLastError = false;

HKEY OpenRegistryKey(HKEY hRootKey, wchar_t* strSubKey)
{
	bRegeditLastError = false;

	HKEY hKey;
	LONG lError = RegOpenKeyEx(hRootKey, strSubKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (ERROR_FILE_NOT_FOUND == lError)
	{
		//Create a key
		lError = RegCreateKeyEx(hRootKey, strSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (lError != ERROR_SUCCESS)
	{
		bRegeditLastError = true;
	}

	return hKey;
}

void SetRegistryValues(HKEY hRootKey, LPCTSTR lpVal, DWORD data)
{
	bRegeditLastError = false;

	LONG nErr = RegSetValueEx(hRootKey, lpVal, NULL, REG_DWORD, (LPBYTE)& data, sizeof(DWORD));
	if (nErr != ERROR_SUCCESS)
	{
		bRegeditLastError = true;
	}
}

void DeleteRegistryKeyAndConfirm(HKEY hRootKey, wchar_t* strSubKey)
{
	bRegeditLastError = false;

	LONG lRes = RegDeleteKey(hRootKey, strSubKey);
	if (ERROR_SUCCESS != lRes)
	{
		bRegeditLastError = true;
	}
}

DWORD GetValueFromRegistry(HKEY hRootKey, LPCTSTR lpValue)
{
	bRegeditLastError = false;

	DWORD data = 0;
	DWORD dtype = REG_DWORD;
	DWORD dSize = sizeof(data);
	LONG lErr = RegQueryValueEx(hRootKey, lpValue, NULL, &dtype, (LPBYTE)& data, &dSize);

	if (ERROR_SUCCESS != lErr)
	{
		bRegeditLastError = true;
	}

	return data;
}

#endif