#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <condition_variable>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <mmsystem.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <dsound.h>

#include "debug.h"
#include "customException.h"
#include "string.h"

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

/*============================================================================================================================================================================================================*

The code has been modified and collected from the following sources and references down below
	+ http://www.cplusplus.com/forum/beginner/88542/ (doesent work?)
	+ http://www.rastertek.com/dx10tut14.html
	+ https://www.youtube.com/watch?v=qGC3xiliJW8 (fun fact i leaked a key and almost got my ass handed to me on stream lol)
	+ https://github.com/gorbit99/olcPGEX_AudioConvert/blob/master/olcPGEX_AudioConverter.h (if only i found this earlier)

	TODO: what is "Waveform Audio API".

/*============================================================================================================================================================================================================*/

#define DIRECT_SOUND_CREATE(name)						HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

using namespace std;

void* pSoundManager;

enum SoundError
{
	SoundLoadSucess = 0,
	SoundLoadLibraryError = 1,
	DirectSoundCreationError,
	FailedToCreatePrimarySoundBuffer,
	FailedToCreateSeconarySoundBuffer,
	FailedToSetWavFormat,
	FailedToOpenFile,
	FailedToReadFile,
	WaveFileNotVerified,
	FailedToSetWaveBuffer,
	FailedToLockTheSecondaryBuffer,
	FailedToUnlockTheSecondaryBuffer,
	FailedToCreate3DBuffer,
	FailedToPlay,
};

struct WaveHeaderType
{
	char						chunkId[4];
	unsigned long				chunkSize;
	char						format[4];
	char						subChunkId[4];
	unsigned long				subChunkSize;
	unsigned short				audioFormat;
	unsigned short				numChannels;
	unsigned long				sampleRate;
	unsigned long				bytesPerSecond;
	unsigned short				blockAlign;
	unsigned short				bitsPerSample;
	char						dataChunkId[4];
	unsigned long				dataSize;

};

struct WaveData
{
	WaveHeaderType				header;
	WAVEFORMATEX				wfWaveFormat;
	DSBUFFERDESC				descSecondaryBuffer, descPrimaryBuffer;
	LPDIRECTSOUNDBUFFER			lpPrimaryBuffer;
	LPDIRECTSOUNDBUFFER			lpSecondaryBuffer;
	LPDIRECTSOUND3DBUFFER		lpSecondary3DBuffer;
	LPDIRECTSOUND3DLISTENER		lpSecondary3DBufferListener;

	unsigned char* waveData;
	unsigned long				waveDataSize;

	//Snippet adapted from https://www.gamedev.net/forums/topic/337397-sound-volume-question-directsound/
	int SetVolume(int volume)
	{
		lpSecondaryBuffer->SetVolume(volume);
		return 0;
	}

	//Snippet adapted from https://www.gamedev.net/forums/topic/337397-sound-volume-question-directsound/
	int SetVolume(int volume, int max)
	{
		lpSecondaryBuffer->SetVolume((int)floorf(2000.0f * log10f((float)(volume) / (float)max) + 0.5f));
		return 0;
	}
};

char* SoundWaveGetEncodingString(short s)
{
	char* st = (char*)malloc(sizeof(char) * 32);

	if (s ==  /*WAVE_FORMAT_PCM*/ 0x0001)						st = (char*)" PCM ";
	if (s ==  /*WAVE_FORMAT_ADPCM*/ 0x0002)						st = (char*)" Microsoft ADPCM Format ";
	if (s ==  /*WAVE_FORMAT_IEEE_FLOAT*/ 0x0003)				st = (char*)" IEEE Float ";
	if (s ==  /*WAVE_FORMAT_VSELP*/ 0x0004)						st = (char*)" Compaq Computer's VSELP ";
	if (s ==  /*WAVE_FORMAT_IBM_CSVD*/ 0x0005)					st = (char*)" IBM CVSD ";
	if (s ==  /*WAVE_FORMAT_ALAW*/ 0x0006)						st = (char*)" ALAW ";
	if (s ==  /*WAVE_FORMAT_MULAW*/ 0x0007)						st = (char*)" MULAW ";
	if (s ==  /*WAVE_FORMAT_OKI_ADPCM*/ 0x0010)					st = (char*)" OKI ADPCM ";
	if (s ==  /*WAVE_FORMAT_DVI_ADPCM*/ 0x0011)					st = (char*)" Intel's DVI ADPCM ";
	if (s ==  /*WAVE_FORMAT_MEDIASPACE_ADPCM*/ 0x0012)			st = (char*)"Videologic's MediaSpace ADPCM";
	if (s ==  /*WAVE_FORMAT_SIERRA_ADPCM*/ 0x0013)				st = (char*)" Sierra ADPCM ";
	if (s ==  /*WAVE_FORMAT_G723_ADPCM*/ 0x0014)				st = (char*)" G.723 ADPCM ";
	if (s ==  /*WAVE_FORMAT_DIGISTD*/ 0x0015)					st = (char*)" DSP Solution's DIGISTD ";
	if (s ==  /*WAVE_FORMAT_DIG	ifIX*/ 0x0016)					st = (char*)" DSP Solution's DIG	ifIX ";
	if (s ==  /*WAVE_FORMAT_DIALOGIC_OKI_ADPCM*/ 0x0017)		st = (char*)" Dialogic OKI ADPCM ";
	if (s ==  /*WAVE_FORMAT_MEDIAVISION_ADPCM*/ 0x0018)			st = (char*)" MediaVision ADPCM ";
	if (s ==  /*WAVE_FORMAT_CU_CODEC*/ 0x0019)					st = (char*)" HP CU ";
	if (s ==  /*WAVE_FORMAT_YAMAHA_ADPCM*/ 0x0020)				st = (char*)" Yamaha ADPCM ";
	if (s ==  /*WAVE_FORMAT_SONARC*/ 0x0021)					st = (char*)" Speech Compression's Sonarc ";
	if (s ==  /*WAVE_FORMAT_TRUESPEECH*/ 0x0022)				st = (char*)" DSP Group's True Speech ";
	if (s ==  /*WAVE_FORMAT_ECHOSC1*/ 0x0023)					st = (char*)" Echo Speech's EchoSC1 ";
	if (s ==  /*WAVE_FORMAT_AUDIOFILE_AF36*/ 0x0024)			st = (char*)" Audiofile AF36 ";
	if (s ==  /*WAVE_FORMAT_APTX*/ 0x0025)						st = (char*)" APTX ";
	if (s ==  /*WAVE_FORMAT_AUDIOFILE_AF10*/ 0x0026)			st = (char*)" AudioFile AF10 ";
	if (s ==  /*WAVE_FORMAT_PROSODY_1612*/ 0x0027)				st = (char*)" Prosody 1612 ";
	if (s ==  /*WAVE_FORMAT_LRC*/ 0x0028)						st = (char*)" LRC ";
	if (s ==  /*WAVE_FORMAT_AC2*/ 0x0030)						st = (char*)" Dolby AC2 ";
	if (s ==  /*WAVE_FORMAT_GSM610*/ 0x0031)					st = (char*)" GSM610 ";
	if (s ==  /*WAVE_FORMAT_MSNAUDIO*/ 0x0032)					st = (char*)" MSNAudio ";
	if (s ==  /*WAVE_FORMAT_ANTEX_ADPCME*/ 0x0033)				st = (char*)" Antex ADPCME ";
	if (s ==  /*WAVE_FORMAT_CONTROL_RES_VQLPC*/ 0x0034)			st = (char*)" Control Res VQLPC ";
	if (s ==  /*WAVE_FORMAT_DIGIREAL*/ 0x0035)					st = (char*)" Digireal ";
	if (s ==  /*WAVE_FORMAT_DIGIADPCM*/ 0x0036)					st = (char*)" DigiADPCM ";
	if (s ==  /*WAVE_FORMAT_CONTROL_RES_CR10*/ 0x0037)			st = (char*)" Control Res CR10 ";
	if (s ==  /*WAVE_FORMAT_VBXADPCM*/ 0x0038)					st = (char*)" NMS VBXADPCM ";
	if (s ==  /*WAVE_FORMAT_ROLAND_RDAC*/ 0x0039)				st = (char*)" Roland RDAC ";
	if (s ==  /*WAVE_FORMAT_ECHOSC3*/ 0x003A)					st = (char*)" EchoSC3 ";
	if (s ==  /*WAVE_FORMAT_ROCKWELL_ADPCM*/ 0x003B)			st = (char*)" Rockwell ADPCM ";
	if (s ==  /*WAVE_FORMAT_ROCKWELL_DIGITALK*/ 0x003C)			st = (char*)" Rockwell Digit LK ";
	if (s ==  /*WAVE_FORMAT_XEBEC*/ 0x003D)						st = (char*)" Xebec ";
	if (s ==  /*WAVE_FORMAT_G721_ADPCM*/ 0x0040)				st = (char*)" Antex Electronics G.721 ";
	if (s ==  /*WAVE_FORMAT_G728_CELP*/ 0x0041)					st = (char*)" G.728 CELP ";
	if (s ==  /*WAVE_FORMAT_MSG723*/ 0x0042)					st = (char*)" MSG723 ";
	if (s ==  /*WAVE_FORMAT_MPEG*/ 0x0050)						st = (char*)" MPEG Layer 1,2 ";
	if (s ==  /*WAVE_FORMAT_RT24*/ 0x0051)						st = (char*)" RT24 ";
	if (s ==  /*WAVE_FORMAT_PAC*/ 0x0051)						st = (char*)" PAC ";
	if (s ==  /*WAVE_FORMAT_MPEGLAYER3*/ 0x0055)				st = (char*)" MPEG Layer 3 ";
	if (s ==  /*WAVE_FORMAT_CIRRUS*/ 0x0059)					st = (char*)" Cirrus ";
	if (s ==  /*WAVE_FORMAT_ESPCM*/ 0x0061)						st = (char*)" ESPCM ";
	if (s ==  /*WAVE_FORMAT_VOXWARE*/ 0x0062)					st = (char*)" Voxware (obsolete) ";
	if (s ==  /*WAVE_FORMAT_CANOPUS_ATRAC*/ 0x0063)				st = (char*)" Canopus Atrac ";
	if (s ==  /*WAVE_FORMAT_G726_ADPCM*/ 0x0064)				st = (char*)" G.726 ADPCM ";
	if (s ==  /*WAVE_FORMAT_G722_ADPCM*/ 0x0065)				st = (char*)" G.722 ADPCM ";
	if (s ==  /*WAVE_FORMAT_DSAT*/ 0x0066)						st = (char*)" DSAT ";
	if (s ==  /*WAVE_FORMAT_DSAT_DISPLAY*/ 0x0067)				st = (char*)" DSAT Display ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_BYTE_ALIGNED */ 0x0069)		st = (char*)" Voxware Byte Aligned (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_AC8*/ 0x0070)				st = (char*)" Voxware AC8 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_AC10*/ 0x0071)				st = (char*)" Voxware AC10 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_AC16*/ 0x0072)				st = (char*)" Voxware AC16 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_AC20*/ 0x0073)				st = (char*)" Voxware AC20 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_RT24*/ 0x0074)				st = (char*)" Voxware MetaVoice (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_RT29*/ 0x0075)				st = (char*)" Voxware MetaSound (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_RT29HW*/ 0x0076)			st = (char*)" Voxware RT29HW (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_VR12*/ 0x0077)				st = (char*)" Voxware VR12 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_VR18*/ 0x0078)				st = (char*)" Voxware VR18 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_TQ40*/ 0x0079)				st = (char*)" Voxware TQ40 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_SOFTSOUND*/ 0x0080)					st = (char*)" Softsound ";
	if (s ==  /*WAVE_FORMAT_VOXWARE_TQ60*/ 0x0081)				st = (char*)" Voxware TQ60 (obsolete) ";
	if (s ==  /*WAVE_FORMAT_MSRT24*/ 0x0082)					st = (char*)" MSRT24 ";
	if (s ==  /*WAVE_FORMAT_G729A*/ 0x0083)						st = (char*)" G.729A ";
	if (s ==  /*WAVE_FORMAT_MVI_MV12*/ 0x0084)					st = (char*)" MVI MV12 ";
	if (s ==  /*WAVE_FORMAT_DF_G726*/ 0x0085)					st = (char*)" DF G.726 ";
	if (s ==  /*WAVE_FORMAT_DF_GSM610*/ 0x0086)					st = (char*)" DF GSM610 ";
	if (s ==  /*WAVE_FORMAT_ISIAUDIO*/ 0x0088)					st = (char*)" ISIAudio ";
	if (s ==  /*WAVE_FORMAT_ONLIVE*/ 0x0089)					st = (char*)" Onlive ";
	if (s ==  /*WAVE_FORMAT_SBC24*/ 0x0091)						st = (char*)" SBC24 ";
	if (s ==  /*WAVE_FORMAT_DOLBY_AC3_SPD	if*/ 0x0092)		st = (char*)" Dolby AC3 SPD	if ";
	if (s ==  /*WAVE_FORMAT_ZYXEL_ADPCM*/ 0x0097)				st = (char*)" ZyXEL ADPCM ";
	if (s ==  /*WAVE_FORMAT_PHILIPS_LPCBB*/ 0x0098)				st = (char*)" Philips LPCBB ";
	if (s ==  /*WAVE_FORMAT_PACKED*/ 0x0099)					st = (char*)" Packed ";
	if (s ==  /*WAVE_FORMAT_RHETOREX_ADPCM*/ 0x0100)			st = (char*)" Rhetorex ADPCM ";
	if (s ==  /*WAVE_FORMAT_IRAT*/ 0x0101)						st = (char*)" BeCubed Software's IRAT ";
	if (s ==  /*WAVE_FORMAT_VIVO_G723*/ 0x0111)					st = (char*)" Vivo G.723 ";
	if (s ==  /*WAVE_FORMAT_VIVO_SIREN*/ 0x0112)				st = (char*)" Vivo Siren ";
	if (s ==  /*WAVE_FORMAT_DIGITAL_G723*/ 0x0123)				st = (char*)" Digital G.723 ";
	if (s ==  /*WAVE_FORMAT_CREATIVE_ADPCM*/ 0x0200)			st = (char*)" Creative ADPCM ";
	if (s ==  /*WAVE_FORMAT_CREATIVE_FASTSPEECH8 */ 0x0202)		st = (char*)" Creative FastSpeech8 ";
	if (s ==  /*WAVE_FORMAT_CREATIVE_FASTSPEECH10 */ 0x0203)	st = (char*)" Creative FastSpeech10 ";
	if (s ==  /*WAVE_FORMAT_QUARTERDECK*/ 0x0220)				st = (char*)" Quarterdeck ";
	if (s ==  /*WAVE_FORMAT_FM_TOWNS_SND*/ 0x0300)				st = (char*)" FM Towns Snd ";
	if (s ==  /*WAVE_FORMAT_BTV_DIGITAL*/ 0x0400)				st = (char*)" BTV Digital ";
	if (s ==  /*WAVE_FORMAT_VME_VMPCM*/ 0x0680)					st = (char*)" VME VMPCM ";
	if (s ==  /*WAVE_FORMAT_OLIGSM*/ 0x1000)					st = (char*)" OLIGSM ";
	if (s ==  /*WAVE_FORMAT_OLIADPCM*/ 0x1001)					st = (char*)" OLIADPCM ";
	if (s ==  /*WAVE_FORMAT_OLICELP*/ 0x1002)					st = (char*)" OLICELP ";
	if (s ==  /*WAVE_FORMAT_OLISBC*/ 0x1003)					st = (char*)" OLISBC ";
	if (s ==  /*WAVE_FORMAT_OLIOPR*/ 0x1004)					st = (char*)" OLIOPR ";
	if (s ==  /*WAVE_FORMAT_LH_CODEC*/ 0x1100)					st = (char*)" LH Codec ";
	if (s ==  /*WAVE_FORMAT_NORRIS*/ 0x1400)					st = (char*)" Norris ";
	if (s ==  /*WAVE_FORMAT_ISIAUDIO*/ 0x1401)					st = (char*)" ISIAudio ";
	if (s ==  /*WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS */ 0x1500)	st = (char*)" Soundspace Music Compression ";
	if (s ==  /*WAVE_FORMAT_DVM*/ 0x2000)						st = (char*)" DVM ";
	if (s ==  /*WAVE_FORMAT_EXTENSIBLE*/ 0xFFFE)				st = (char*)" SubFormat ";
	if (s ==  /*WAVE_FORMAT_DEVELOPMENT*/ 0xFFFF)				st = (char*)" Development ";

	return st;
}

class SoundManager
{
private:
	direct_sound_create* DirectSoundCreate;

public:
	HMODULE DSoundLibrary;
	LPDIRECTSOUND lpdsDirectSound;

	vector<WaveData*> vSounds;

	bool bInialized;

	int Initalize()
	{
		odprintf("Initalizing SoundManager[%p]...", this);

		debugTabIndex++;

		odprintf("Loading the directx sound library .dll ...");
		DSoundLibrary = LoadLibraryA("library\\dsound\\dsound.dll");							//NOTE: There is a much easier way and a much simpler way to implement this but my babyass wants totalteriann control/info 

		if (!DSoundLibrary)
		{
			odprintf("ERROR: Failed to load the sound library!");
			return SoundLoadLibraryError;																//If we did fail to load return print and return
		}

		// NOTE(casey): Get a DirectSound object! - cooperative
		odprintf("Loading the funtions from the dll...");

		debugTabIndex++;

		odprintf("HRESULT WINAPI DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)");
		DirectSoundCreate = (direct_sound_create*)														//NOTE: HOW THE ACTUAL FUck is this working while watching the tutorial i rewrote this code multiple times
			GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		debugTabIndex--;

		odprintf("Done!");

		if (FAILED(DirectSoundCreate(NULL, &lpdsDirectSound, NULL)))
		{
			odprintf("ERROR: Failed to create DirectSound object!");
			debugTabIndex--;
			return DirectSoundCreationError;
		}

		pSoundManager = this;

		debugTabIndex--;

		odprintf("Initalized SoundManager[%p]!", this);

		return 0;
	}

	int LoadSoundFile(wstring name, bool bare, bool b2d, bool b3d)
	{
		odprintf("SoundManager[%p].LoadSoundFile(%ls)...", this, name.c_str());
		debugTabIndex++;

		WaveData* wdCurrent = new WaveData();
		long bitrate = 36000;																			//Smallest bitrate
		FILE* filePtr = nullptr;

		odprintf("Opening the file...");																//Set the buffer for the primary and secondary buffers
		int iResult = _wfopen_s(&filePtr, name.c_str(), L"rb");

		if (iResult != 0 || filePtr == nullptr)
		{
			odprintf("ERROR: Failed to open the file!");												//Print the error and return
			debugTabIndex--;
			return FailedToOpenFile;
		}

		odprintf("Reading the wave header struct...");													//Read the wave structure
		iResult = fread(&wdCurrent->header, sizeof(wdCurrent->header), 1, filePtr);						//Call the function

		if (iResult != 1)
		{
			odprintf("ERROR: Failed to read the file!");												//Print the error and return
			debugTabIndex--;
			return FailedToReadFile;
		}

		odprintf("Verifying the wave header struct...");												//Verify the wave structure
		// Check that the chunk ID is the RIFF format.
		if ((wdCurrent->header.chunkId[0] != 'R') || (wdCurrent->header.chunkId[1] != 'I') ||
			(wdCurrent->header.chunkId[2] != 'F') || (wdCurrent->header.chunkId[3] != 'F'))
		{
			odprintf("ERROR: Failed to verify the RIIF format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		// Check that the file format is the WAVE format.
		if ((wdCurrent->header.format[0] != 'W') || (wdCurrent->header.format[1] != 'A') ||
			(wdCurrent->header.format[2] != 'V') || (wdCurrent->header.format[3] != 'E'))
		{
			odprintf("ERROR: Failed to verify the RIIF format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		// Check that the sub chunk ID is the fmt format.
		if ((wdCurrent->header.subChunkId[0] != 'f') || (wdCurrent->header.subChunkId[1] != 'm') ||
			(wdCurrent->header.subChunkId[2] != 't') || (wdCurrent->header.subChunkId[3] != ' '))
		{
			odprintf("ERROR: Failed to verify the fmt format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		odprintf("Setting cooperative level...");
		lpdsDirectSound->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);						//TODO: Make some debugging because this may fail?

		odprintf("Setting the buffer description...");													//Set the buffer for the primary and secondary buffers
		ZeroMemory(&wdCurrent->descPrimaryBuffer, sizeof(wdCurrent->descPrimaryBuffer));										//Clean the buffers with zero

		//Set the primary buffer identities
		wdCurrent->descPrimaryBuffer.dwSize = sizeof(wdCurrent->descPrimaryBuffer);											//They may change the sizes in l8r versions
		wdCurrent->descPrimaryBuffer.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;

		odprintf("Createing the sound buffers...");														//Set the buffer for the primary and secondary buffers
		if (FAILED(lpdsDirectSound->CreateSoundBuffer(&wdCurrent->descPrimaryBuffer, &wdCurrent->lpPrimaryBuffer, 0)))
		{
			odprintf("ERROR: Failed to create a primary sound buffer!");								//Print the error and return
			debugTabIndex--;
			return FailedToCreatePrimarySoundBuffer;
		}

		odprintf("Verified!");

		odprintf("Wave file encoding type 0x%04x, %s", wdCurrent->header.audioFormat, SoundWaveGetEncodingString(wdCurrent->header.audioFormat));

		odprintf("Setting the .wav format...");															//Set the default wav formatWAVE_FORMAT_IEEE_FLOAT		
		wdCurrent->wfWaveFormat.wFormatTag = wdCurrent->header.audioFormat;
		wdCurrent->wfWaveFormat.nChannels = wdCurrent->header.numChannels;
		wdCurrent->wfWaveFormat.nSamplesPerSec = wdCurrent->header.sampleRate;
		wdCurrent->wfWaveFormat.wBitsPerSample = wdCurrent->header.bitsPerSample;
		wdCurrent->wfWaveFormat.nBlockAlign = (wdCurrent->wfWaveFormat.nChannels * wdCurrent->wfWaveFormat.wBitsPerSample) / 8;
		wdCurrent->wfWaveFormat.nAvgBytesPerSec = wdCurrent->wfWaveFormat.nSamplesPerSec * wdCurrent->wfWaveFormat.nBlockAlign;
		wdCurrent->wfWaveFormat.cbSize = 0;

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		odprintf("Reading the file into the buffer[%lu]...", wdCurrent->header.dataSize);				//Set the default wav format
		fseek(filePtr, 0, SEEK_SET);

		wdCurrent->waveData = new unsigned char[wdCurrent->header.dataSize];
		if (!wdCurrent->waveData)
		{
			odprintf("ERROR: Failed to create a char buffers!");										//Print the error and return
			debugTabIndex--;
			return FailedToSetWaveBuffer;
		}

		while (true)
		{
			char c[4];
			fread(c, 1, 1, filePtr);

			if (c[0] == 'd')
			{
				fread(c + 1, 1, 3, filePtr);

				if (c[0] == 'd' && c[1] == 'a' && c[2] == 't' && c[3] == 'a') break;
			}
		}

		iResult = fread(wdCurrent->waveData, sizeof(unsigned char), wdCurrent->header.dataSize, filePtr);

		if (iResult != wdCurrent->header.dataSize)
		{
			odprintf("ERROR: Failed to load!");															//Print the error and return
			debugTabIndex--;
			return FailedToSetWaveBuffer;
		}

		iResult = fclose(filePtr);
		if (FAILED(iResult))
		{
			odprintf("WARNING: file pointer failed to close! May cause memory leakage\\future errors");	//Print the error and return
		}


		odprintf("Setting the buffer description...");													//Set the buffer for the primary and secondary buffers
		ZeroMemory(&wdCurrent->descSecondaryBuffer, sizeof(wdCurrent->descSecondaryBuffer));

		//Set the secondary buffer 
		wdCurrent->descSecondaryBuffer.dwSize = sizeof(wdCurrent->descSecondaryBuffer);
		wdCurrent->descSecondaryBuffer.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
		wdCurrent->descSecondaryBuffer.dwBufferBytes = wdCurrent->header.dataSize;
		wdCurrent->descSecondaryBuffer.dwReserved = 0;
		wdCurrent->descSecondaryBuffer.lpwfxFormat = &wdCurrent->wfWaveFormat;
		wdCurrent->descSecondaryBuffer.guid3DAlgorithm = GUID_NULL;

		odprintf("Creating the secondary sound buffer...");												//Set the default wav format
		if (FAILED(lpdsDirectSound->CreateSoundBuffer(&wdCurrent->descSecondaryBuffer, &wdCurrent->lpSecondaryBuffer, 0)))
		{
			odprintf("ERROR: Failed to create a secondary sound buffer!");								//Print the error and return
			debugTabIndex--;
			return FailedToCreatePrimarySoundBuffer;
		}

		// Copy the wave data into the buffer.
		unsigned char* bufferPtr;																		//The pointer to the DirectSounds buffers
		unsigned long bufferSize;

		odprintf("Copying to the secondary sound buffer");
		iResult = wdCurrent->lpSecondaryBuffer->Lock(0, wdCurrent->header.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to lock the secondary buffer!");									//Print the error and return
			debugTabIndex--;
			return FailedToLockTheSecondaryBuffer;
		}

		memcpy(bufferPtr, wdCurrent->waveData, wdCurrent->header.dataSize);

		iResult = wdCurrent->lpSecondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to unlock the secondary buffer!");									//Print the error and return
			debugTabIndex--;
			return FailedToUnlockTheSecondaryBuffer;
		}

		odprintf("Coppied!");

		if (b3d)
		{
			// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
			odprintf("Getting the 3D interface of the buffer!");
			iResult = wdCurrent->lpSecondaryBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&wdCurrent->lpSecondary3DBuffer);
			if (FAILED(iResult))
			{
				odprintf("ERROR: Failed to get the 3D buffer!");											//Print the error and return
				debugTabIndex--;
				return FailedToCreate3DBuffer;
			}

			iResult = wdCurrent->lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&wdCurrent->lpSecondary3DBufferListener);
			if (FAILED(iResult))
			{
				odprintf("ERROR: Failed to get the 3D listener!");											//Print the error and return
				debugTabIndex--;
				return FailedToCreate3DBuffer;
			}

			wdCurrent->lpSecondary3DBufferListener->SetDistanceFactor(1, DS3D_IMMEDIATE);
			wdCurrent->lpSecondary3DBufferListener->SetDopplerFactor(1, DS3D_IMMEDIATE);
			wdCurrent->lpSecondary3DBufferListener->SetRolloffFactor(1, DS3D_IMMEDIATE);

			wdCurrent->lpSecondary3DBufferListener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

		}

		odprintf("Cleaning up...");

		if (bare)
		{
			delete[] wdCurrent->waveData;
			wdCurrent->waveDataSize = sizeof(wdCurrent->waveData);
		}

		if (b2d)
		{
			wdCurrent->lpSecondaryBuffer->SetCurrentPosition(0);
			wdCurrent->lpSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
		}
		else
		{
			wdCurrent->lpSecondaryBuffer->Release();
		}

		vSounds.push_back(wdCurrent);

		debugTabIndex--;

		return 0;
	}

	LPDIRECTSOUNDBUFFER		GetPrimaryBuffer(bool b3d)
	{
		DSBUFFERDESC descPrimary;
		LPDIRECTSOUNDBUFFER lpPrimaryBuffer;

		odprintf("Setting the buffer description...");													//Set the buffer for the primary and secondary buffers
		ZeroMemory(&descPrimary, sizeof(descPrimary));													//Clean the buffers with zero

		//Set the primary buffer identities
		descPrimary.dwSize = sizeof(descPrimary);														//They may change the sizes in l8r versions

		if (b3d)
			descPrimary.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
		else
			descPrimary.dwFlags = DSBCAPS_PRIMARYBUFFER;

		odprintf("Createing the sound buffers...");														//Set the buffer for the primary and secondary buffers
		if (FAILED(lpdsDirectSound->CreateSoundBuffer(&descPrimary, &lpPrimaryBuffer, 0)))
		{
			odprintf("ERROR: Failed to create a primary sound buffer!");								//Print the error and return
			debugTabIndex--;
			return NULL;
		}

		return lpPrimaryBuffer;

	}

	int Cleanup()
	{
		odprintf("Cleaning SoundManager[%p]...", this);

		debugTabIndex++;

		lpdsDirectSound->Release();

		for (int i = 0; i < vSounds.size(); i++)
		{
			vSounds[i]->lpSecondaryBuffer->Release();
			delete[] vSounds[i]->waveData;

			delete vSounds[i];

		}

		vSounds.erase(vSounds.begin(), vSounds.end());

		debugTabIndex--;

	}

};

//Needed for easier reading
#define SoundMacro ((SoundManager*)pSoundManager)

//Cool macros cuz im lazy
#define SoundMacroLoad(iResult, wavdata, name, bare, b2d, b3d)								\
iResult = SoundMacro->LoadSoundFile(name, bare, b2d, b3d);									\
odprintf("Loaded sound [%s] with the iResult [%i]!", to_string(name).c_str(), iResult);		\
if (iResult == 0)																			\
{																							\
	wavdata = SoundMacro->vSounds[SoundMacro->vSounds.size() - 1];							\
}																							\

#endif