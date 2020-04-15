//#ifndef OPENVR_WRAPPER_H
//#define OPENVR_WRAPPER_H
//
//#include <iostream>
//#include <string>
//#include <Windows.h>
//
//#include "debug.h"
//#include "library/openvr/headers/openvr.h"
//
///** Returns true if there is an HMD attached. This check is as lightweight as possible and can be called outside of VR_Init/VR_Shutdown. It should be used when an application want to know if initializing VR is a possibility but isn't ready to take that step yet.	*/
//#define OVR_IsHmdPresent(name)									bool VR_CALLTYPE name();		
///** Returns true if the OpenVR runtime is installed. */
//#define OVR_IsRuntimeInstalled(name)							bool VR_CALLTYPE name();		
///** Returns where the OpenVR runtime is installed. */
//#define OVR_GetRuntimePath(name)								bool name(VR_OUT_STRING() char* pchPathBuffer, uint32_t unBufferSize, uint32_t* punRequiredBufferSize);
///** Returns the name of the enum value for an EVRInitError. This function may be called outside of VR_Init()/VR_Shutdown(). */
//#define OVR_GetVRInitErrorAsSymbol(name)						const char* VR_CALLTYPE name(vr::EVRInitError error);
///** Returns an English string for an EVRInitError. Applications should call VR_GetVRInitErrorAsSymbol instead and
//* use that as a key to look up their own localized error message. This function may be called outside of VR_Init()/VR_Shutdown(). */
//#define OVR_GetVRInitErrorAsEnglishDescription(name)			const char* VR_CALLTYPE name(vr::EVRInitError error);
///** Returns the interface of the specified version. This method must be called after VR_Init. The
//* pointer returned is valid until VR_Shutdown is called.*/
//#define OVR_GetGenericInterface(name)							void* VR_CALLTYPE name(const char* pchInterfaceVersion, vr::EVRInitError* peError);
///** Returns whether the interface of the specified version exists.*/
//#define OVR_IsInterfaceVersionValid(name)						bool VR_CALLTYPE name(const char* pchInterfaceVersion);
///** Returns a token that represents whether the VR interface handles need to be reloaded */
//#define OVR_GetInitToken(name)									uint32_t VR_CALLTYPE name();
//
//typedef OVR_IsHmdPresent										(ovr_IsHmdPresent);
//typedef OVR_IsRuntimeInstalled									(ovr_IsRuntimeInstalled);
//typedef OVR_GetRuntimePath										(ovr_GetRuntimePath);
//typedef OVR_GetVRInitErrorAsSymbol								(ovr_GetVRInitErrorAsSymbol);
//typedef OVR_GetVRInitErrorAsEnglishDescription					(ovr_GetVRInitErrorAsEnglishDescription);
//typedef OVR_GetGenericInterface									(ovr_GetGenericInterface);
//typedef OVR_IsInterfaceVersionValid								(ovr_IsInterfaceVersionValid);
//typedef OVR_GetInitToken										(ovr_GetInitToken);
//
//class OpenVrManager
//{
//private:
//	HMODULE hOpenVRLibrary;
//	
//	vr::IVRSystem* ivrContext;
//
//	ovr_IsHmdPresent*						func_IsHmdPresent;
//	ovr_IsRuntimeInstalled*					func_IsRuntimeInstalled;
//	ovr_GetRuntimePath*						func_GetRuntimePath;
//	ovr_GetVRInitErrorAsSymbol*				func_GetVRInitErrorAsSymbol;
//	ovr_GetVRInitErrorAsEnglishDescription* func_GetVRInitErrorAsEnglishDescription;
//	ovr_GetGenericInterface*				func_GetGenericInterface;
//	ovr_IsInterfaceVersionValid*			func_IsInterfaceVersionValid;
//	ovr_GetInitToken*						func_GetInitToken;
//
//public:
//	int Initalize()
//	{
//		odprintf("Initalizing OpenVrManager[%p]...", this);
//		debugTabIndex++;
//
//		//Load the main dll
//		std::string pathOfDll = "";
//		IF32BIT
//			pathOfDll = "library\\openvr\\bin\\win32\\openvr_api.dll";												//NOTE: There is a much easier way and a much simpler way to implement this but my babyass wants totalteriann control/info 
//		
//		IF64BIT
//			pathOfDll = "library\\openvr\\bin\\win64\\openvr_api.dll";												//NOTE: There is a much easier way and a much simpler way to implement this but my babyass wants totalteriann control/info 
//			
//		odprintf("Attempting to load DLL %s...", pathOfDll.c_str());
//
//		hOpenVRLibrary = LoadLibraryA(pathOfDll.c_str());
//
//		if (!hOpenVRLibrary)
//		{
//			odprintf("ERROR: Failed to load the library!");
//			return vr::VRInitError_Init_VRClientDLLNotFound;														//If we did fail to load return print and return
//		}
//
//		odprintf("Loading the pointers to the funcitons...");
//		//Load the functions needed from the dll
//		func_IsHmdPresent							= (ovr_IsHmdPresent*) GetProcAddress(hOpenVRLibrary, "VR_IsHmdPresent");
//		func_IsRuntimeInstalled						= (ovr_IsRuntimeInstalled*) GetProcAddress(hOpenVRLibrary, "VR_IsRuntimeInstalled");
//		func_GetRuntimePath							= (ovr_GetRuntimePath*) GetProcAddress(hOpenVRLibrary, "VR_GetRuntimePath");
//		func_GetVRInitErrorAsSymbol					= (ovr_GetVRInitErrorAsSymbol*) GetProcAddress(hOpenVRLibrary, "VR_GetVRInitErrorAsSymbol");
//		func_GetVRInitErrorAsEnglishDescription		= (ovr_GetVRInitErrorAsEnglishDescription*) GetProcAddress(hOpenVRLibrary, "VR_GetVRInitErrorAsEnglishDescription");
//		func_GetGenericInterface					= (ovr_GetGenericInterface*) GetProcAddress(hOpenVRLibrary, "VR_GetGenericInterface");
//		func_IsInterfaceVersionValid				= (ovr_IsInterfaceVersionValid*) GetProcAddress(hOpenVRLibrary, "VR_IsInterfaceVersionValid");
//		func_GetInitToken							= (ovr_GetInitToken*) GetProcAddress(hOpenVRLibrary, "VR_GetInitToken");
//
//		if (
//			func_IsHmdPresent == nullptr &&
//			func_IsRuntimeInstalled == nullptr &&
//			func_GetRuntimePath == nullptr &&
//			func_GetVRInitErrorAsSymbol == nullptr &&
//			func_GetVRInitErrorAsSymbol == nullptr &&
//			func_GetVRInitErrorAsEnglishDescription == nullptr &&
//			func_GetGenericInterface == nullptr &&
//			func_IsInterfaceVersionValid == nullptr &&
//			func_GetInitToken == nullptr
//			)
//		{
//			odprintf("ERROR: Failed to load the library!");
//			return vr::VRInitError_Init_InstallationCorrupt;														//If we did fail to load return print and return
//		}
//
//		//Check whether there’s an HMD connected and a runtime installed 
//		if (func_IsHmdPresent() && func_IsRuntimeInstalled())
//			odprintf("ovr is connected and ready!");
//		else
//		{
//			if (!func_IsHmdPresent()) odprintf("ERROR: Failed to connect to HMD!");
//			if (!func_IsRuntimeInstalled()) odprintf("ERROR: Failed to find runtime!");
//			return vr::VRInitError_Driver_NotLoaded;																//If we did fail to load return print and return
//		}
//
//		//Load the SteamVR runtime context
//		vr::EVRInitError err;
//		ivrContext = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene);
//
//		if (ivrContext == nullptr)
//		{
//			odprintf("ERROR: failed to initalize SteamVR [%d]!", (int)err);
//			return err;
//		}
//
//		return vr::VRInitError_None;
//	}
//};
//
//#endif