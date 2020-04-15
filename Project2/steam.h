#ifndef STEAM_H
#define STEAM_H

//
//#include "api/steam/steam_api.h"
//
///*============================================================================================================================================================================================================*
//
//The code has been modified and collected from the following sources and references down below
//	+ HEAVILLY modified from sound.h
//	+ https://partner.steamgames.com/doc/api/steam_api#SteamAPI_Init
//
///*============================================================================================================================================================================================================*/
//
//
////	#define S_API extern "C" __declspec( dllimport ) 
//
////Steam defines and typedefs
//#define STEAM_INITALIZE(name)									bool S_CALLTYPE name();
//#define STEAM_RESTARTAPPIFNECESSARY(name)						bool S_CALLTYPE name( uint32 unOwnAppID );
//typedef STEAM_INITALIZE(Function_SteamAPI_Init);
//typedef STEAM_RESTARTAPPIFNECESSARY(Function_SteamAPI_RestartAppIfNecessary);
//
//enum SteamManagerError
//{
//	SteamLoadLibraryError = 1,
//	SteamApiFunctionLoadError,
//	SteamApiInitalizationError,
//};
//
//class SteamManager
//{
//private:
//	Function_SteamAPI_Init* SteamAPI_Init;
//	Function_SteamAPI_RestartAppIfNecessary* SteamAPI_RestartAppIfNecessary;
//
//public:
//
//	int Initalize()
//	{
//		odprintf("Initalizing SteamManager[%p]...", this);
//		debugTabIndex++;
//
//		//Load the dll file
//		odprintf("Loading the steam_api.dll...");														
//
//		HMODULE DSteamLibrary = LoadLibraryA("steam_api64.dll");										//NOTE: There is a much easier way and a much simpler way to implement this but my babyass wants totalteriann control/info 
//
//		if (!DSteamLibrary)																				//Test if we got a ret
//		{
//			odprintf("ERROR: Failed to load the steam library!");
//			return SteamLoadLibraryError;																//If we did fail to load return print and return
//		}
//
//		//Load the functions that we need from the dll
//		odprintf("Loading the funtions from the dll...");												
//
//		debugTabIndex++;
//
//		odprintf("bool S_CALLTYPE SteamAPI_Init();");															//bool S_CALLTYPE SteamAPI_Init();
//		SteamAPI_Init = (Function_SteamAPI_Init*)
//			GetProcAddress(DSteamLibrary, "SteamAPI_Init");
//		odprintf("bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 unOwnAppID );");						//bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 unOwnAppID );
//		SteamAPI_RestartAppIfNecessary = (Function_SteamAPI_RestartAppIfNecessary*)
//			GetProcAddress(DSteamLibrary, "SteamAPI_RestartAppIfNecessary");
//
//		debugTabIndex--;
//
//		odprintf("Testing for loading errors...");
//
//		if (SteamAPI_Init == nullptr)
//		{
//			odprintf("ERROR: Failed to load [bool S_CALLTYPE SteamAPI_Init();]! May cause some unwanted errors");
//			debugTabIndex--;
//			return SteamApiFunctionLoadError;
//		}
//
//		if (SteamAPI_RestartAppIfNecessary == nullptr)
//		{
//			odprintf("ERROR: Failed to load [bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 unOwnAppID );]! May cause some unwanted errors");
//			debugTabIndex--;
//			return SteamApiFunctionLoadError;
//		}
//
//		//Initalize the SteamApi;
//		odprintf("Initalizing the SteamApi()...");
//		int iResult = SteamAPI_Init();
//
//		if (FAILED(iResult))
//		{
//			odprintf("ERROR: Failed to initalize the steam api1");
//			debugTabIndex--;
//			return SteamApiInitalizationError;
//		}
//
//		odprintf("Done!");
//
//		//End and return
//		debugTabIndex--;
//		odprintf("Initalized SteamManager[%p]!", this);
//
//		return 0;
//	}
//
//};

#endif