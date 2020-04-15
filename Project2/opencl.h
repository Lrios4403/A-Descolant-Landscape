/*===INFOMATION=============================================================================*/
/*Programmed by: Lrios4403
/*Date:
/*Copyright: none (dont know and dont care)
/*Notes: +Why do people put these at the top of every script?
/*==========================================================================================*/

#ifndef OPENCL_H
#define OPENCL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 1
#define CL_HPP_ENABLE_EXCEPTIONS 

#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cinttypes>

#include "debug.h"

using namespace std;

/*===TERMINOLOGY============================================================================*/
/*
/* Platform: The OpenCL version.
/* Context: Devices selected to work together
/* Device: Physical devices supporting OpenCl
/* Host: the application code
/* Kernel: the gpu code (compiled
/* Work Item: a unit of work executing a kernel
/* Work Groups: a collection of work items
/* Command Que: the only way to communicate with a device
/* Memory: Local/Global/Private/Constant
/* Buffer: An area of memory on a gpu
/* Compute Unit: A work group
/*
/*==========================================================================================*/

enum OpenCLVersion
{
	OpenCLVersionv10 = 0,
	OpenCLVersionv11 = 1,
	OpenCLVersionv12 = 2,
	OpenCLVersionv20 = 3,
	OpenCLVersionv21 = 4,
	OpenCLVersionv22 = 5
};

vector<string> clInvalidPlatforms;
cl::Platform clCurrentPlatform;
cl::Device clCurrentDevice;
cl::CommandQueue clCurrentCommandQue;
cl::Context clCurrentContext;
OpenCLVersion clCurrentVersion;
string clOpenCLCVersion;
string clCurrentDeviceBuiltInKernels;

const char* getErrorString(cl_int error);

int clInitalize()
{
	printf("\nInitalizing OpenCL...");
	auto start = std::chrono::steady_clock::now();

	// Filter for a 2.0 platform and set it as the default
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Device bestDevice;
	cl::Platform::get(&platforms);

	printf("\nChecking device platforms...");													//Check for platforms
recheckPlatforms:

	if (platforms.size() == 0)																	//If there were no platforms (i.e. there were no devices supporting opencl)
	{
		printf("\nERROR: No OpenCL devices found!");											//Debug and return with an error
		return -1;
	}

	OpenCLVersion currentV = OpenCLVersionv10;
	for (auto& p : platforms)
	{
		bool cont = false;
		for (auto& b : clInvalidPlatforms)
		{
			if (p.getInfo<CL_PLATFORM_VERSION>() == b) cont = true;
		}

		if (cont)
		{
			std::string platver = p.getInfo<CL_PLATFORM_VERSION>();								//Get the current platform 
			printf("\n       %s INVALID", platver.c_str());										//Simple debug output
			continue;
		}

		std::string platver = p.getInfo<CL_PLATFORM_VERSION>();									//Get the current platform 
		printf("\n       %s", platver.c_str());													//Simple debug output

		/*
		if (platver.find("OpenCL 2.") != std::string::npos) {
			clCurrentPlatform = p;
		}
		*/

		if (platver.find("OpenCL 1.0") != std::string::npos) currentV = OpenCLVersionv10;
		if (platver.find("OpenCL 1.1") != std::string::npos) currentV = OpenCLVersionv11;
		if (platver.find("OpenCL 1.2") != std::string::npos) currentV = OpenCLVersionv12;
		if (platver.find("OpenCL 2.0") != std::string::npos) currentV = OpenCLVersionv20;
		if (platver.find("OpenCL 2.1") != std::string::npos) currentV = OpenCLVersionv21;
		if (platver.find("OpenCL 2.2") != std::string::npos) currentV = OpenCLVersionv22;

		if (currentV > clCurrentVersion && !cont)
		{
			clCurrentVersion = currentV;
			clCurrentPlatform = p;
		}

	}

	if (clCurrentVersion == OpenCLVersionv10) printf("\nCurrent OpenCl version: 1.0 | WARNING THIS VERSION IS OLD!");	//Debug outut for runetime
	if (clCurrentVersion == OpenCLVersionv11) printf("\nCurrent OpenCl version: 1.1 | WARNING THIS VERSION IS OLD!");
	if (clCurrentVersion == OpenCLVersionv12) printf("\nCurrent OpenCl version: 1.2 | WARNING THIS VERSION IS OLD!");
	if (clCurrentVersion == OpenCLVersionv20) printf("\nCurrent OpenCl version: 2.0");
	if (clCurrentVersion == OpenCLVersionv22) printf("\nCurrent OpenCl version: 2.2");
	if (clCurrentVersion == OpenCLVersionv22) printf("\nCurrent OpenCl version: 2.2");

	printf("\nGetting all devices...");															//Get all of the devices (gpu/cpu)
	clCurrentPlatform.getDevices(CL_DEVICE_TYPE_ALL, &devices);									//^^^
	if (devices.size() == 0)																	//If there were no devices (strange since we got a platform)
	{
		printf("\nERROR: No OpenCL devices found! (Wierd since we got a platform?)");			//Return an error
		return -1;
	}

	bestDevice = cl::Device();																	//Set the best device for the front (so we have something to compare)

	printf("\nChecking for best device...");													//Get all of the devices (gpu/cpu)
	std::ifstream helloWorldFile("HelloWorld.cl");
	std::string src(std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()));
	
	cl::Program::Sources testProgramSource(1, std::make_pair(src.c_str(), src.length() + 1));

	for (auto& d : devices)																		//Look through all devices to see which is best
	{
		printf("\nChecking device: %s | %s", d.getInfo<CL_DEVICE_NAME>().c_str(), d.getInfo<CL_DEVICE_VENDOR>().c_str());

		//First look if the device is usable
		if (!d.getInfo<CL_DEVICE_AVAILABLE>() || !d.getInfo<CL_DEVICE_COMPILER_AVAILABLE>()) break;

		//Since we are doing paralell processing (not so much brute computing) we are going to look at MaxComputeUnits, MaxClockFrequency, MaxMemoryAllocationSize
		//TODO: Look into the OpenCL built in kernels
		int scoreD1 = 0, scoreD2 = 0;															//To keep score at out devices
		int maxCpuD1;
		int maxCpuD2;

		//Can the current device compile the kernel code WITH the current opencl infomation?
		cl::Context context(d);
		cl::Program program(context, testProgramSource);
		cl_int clErrorCode = 0;

		if (clCurrentVersion == OpenCLVersionv10) clErrorCode = program.build("-cl-std=CL1.0");	//Cycle through all the verisons of opencl and build accordingly
		if (clCurrentVersion == OpenCLVersionv11) clErrorCode = program.build("-cl-std=CL1.1");
		if (clCurrentVersion == OpenCLVersionv12) clErrorCode = program.build("-cl-std=CL1.2");
		if (clCurrentVersion == OpenCLVersionv20) clErrorCode = program.build("-cl-std=CL2.0");
		if (clCurrentVersion == OpenCLVersionv22) clErrorCode = program.build("-cl-std=CL2.1");
		if (clCurrentVersion == OpenCLVersionv22) clErrorCode = program.build("-cl-std=CL2.2");

		if (clErrorCode < 0)																	//The test code could not be compiled by the current device, display error and continue
		{
			printf("\n       ERROR: %s", getErrorString(clErrorCode));
			continue;
		}

		//Ok so the kernel code was compiled successfully, now lets try an execute it
		char buf[16];
		cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
		cl::Kernel kernel(program, "Hello", &clErrorCode);
		kernel.setArg(0, memBuf);

		cl::CommandQueue queue(context, d);
		queue.enqueueTask(kernel);
		queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

		if (clErrorCode < 0)																	//The test code could not run on the current device
		{
			printf("\n       ERROR: %s", getErrorString(clErrorCode));
			continue;
		}

		//If the bestDevice() is still empty just fill it
		if (bestDevice() == NULL)
		{
			bestDevice = d;
			continue;
		}

		//Lets go through this list shall we
		//Check MaxComputeUnits * MaxClockFrequency
		maxCpuD1 = d.getInfo <CL_DEVICE_MAX_CLOCK_FREQUENCY>() * d.getInfo <CL_DEVICE_MAX_COMPUTE_UNITS>();
		maxCpuD2 = bestDevice.getInfo <CL_DEVICE_MAX_CLOCK_FREQUENCY>() * bestDevice.getInfo <CL_DEVICE_MAX_COMPUTE_UNITS>();

		if (maxCpuD1 > maxCpuD2) bestDevice = d;
	}

	if (bestDevice() == NULL)																	//If forwhatever reason non of devices worked
	{
		clInvalidPlatforms.push_back(clCurrentPlatform.getInfo<CL_PLATFORM_VERSION>());																	//Push the current platform as invalid
		printf("\nERROR: The current platform %s is incompatible! Redoing...", clCurrentPlatform.getInfo<CL_PLATFORM_VERSION>().c_str());				//Display error message
		clCurrentPlatform = cl::Platform();																												//Clear the platform
		clCurrentVersion = OpenCLVersionv10;																											//Reset the verison
		goto recheckPlatforms;
	}

	clCurrentDevice = bestDevice;																														//We got out best device
	clCurrentContext = cl::Context(clCurrentDevice);																									//Set the context of our device

	printf("\nCollected best device: %s | %s", clCurrentDevice.getInfo<CL_DEVICE_NAME>().c_str(), clCurrentDevice.getInfo<CL_DEVICE_VENDOR>().c_str());

	//Simple debugging to tell if the device supports certain float parameters
	cl_device_fp_config clFloatStatus = clCurrentDevice.getInfo< CL_DEVICE_DOUBLE_FP_CONFIG>();															//Get the status

	if (!clFloatStatus& (1 << (0))) printf("\nNOT SUPPORTED       CL_FP_DENORM:           - denorms are supported.");
	if (!clFloatStatus& (1 << (1))) printf("\nNOT SUPPORTED       CL_FP_INF_NAN:          - INF and NaNs are supported.");
	if (!clFloatStatus& (1 << (2))) printf("\nNOT SUPPORTED       CL_FP_ROUND_TO_NEAREST: - round to nearest even rounding mode supported.");
	if (!clFloatStatus& (1 << (3))) printf("\nNOT SUPPORTED       CL_FP_ROUND_TO_ZERO:    - round to zero rounding mode supported.");
	if (!clFloatStatus& (1 << (4))) printf("\nNOT SUPPORTED       CL_FP_ROUND_TO_INF:     - round to positiveand negative infinity rounding modes supported.");
	if (!clFloatStatus& (1 << (5))) printf("\nNOT SUPPORTED       CL_FP_FMA:              - IEEE754 - 2008 fused multiply - add is supported.");
	if (!clFloatStatus& (1 << (6))) printf("\nNOT SUPPORTED       CL_FP_SOFT_FLOAT:       - Basic floating - point operations(such as addition, subtraction, multiplication) are implemented in software.");

	//Check the type of OpenCL C version supported
	clOpenCLCVersion = clCurrentDevice.getInfo<CL_DEVICE_OPENCL_C_VERSION>();																			//Get the current opencl c version
	printf("\n       OpenCL C Version: %s", clOpenCLCVersion.c_str());																					//Print the current opencl c version
	
	//Look at all of the prebuilt kernels
	clOpenCLCVersion = clCurrentDevice.getInfo<CL_DEVICE_BUILT_IN_KERNELS>();																			//Get the current opencl c version
	printf("\n       Prebuilt Kernels: %s", clOpenCLCVersion.c_str());																					//Print the current opencl c version


	//Now we need to set the command que for the current device
	printf("\nLoading the command que for the current device");									//Simple debug message
	clCurrentCommandQue = cl::CommandQueue(clCurrentContext, clCurrentDevice);
	
	auto end = chrono::steady_clock::now();
	printf("\nOpenCL has been initalized! %" PRId64 "ms", chrono::duration_cast<chrono::milliseconds>(end - start).count());
	printf("\n");

	return 0;
}

/*===NOTE===================================================================================*/
/* This funciton is a little tool to load functions from .cl files, reduces code but takes
/* mode time.
*/

cl::Kernel clLoadKernel(string funcname, string filename)																			
{
	printf("\nLoading KERNEL %s %s", funcname.c_str(), filename.c_str());
	
	std::ifstream ffile(filename.c_str());

	std::string src(std::istreambuf_iterator<char>(ffile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Program program(clCurrentContext, sources);

	cl_int clErrorCode = 0;
	if (clCurrentVersion == OpenCLVersionv10) clErrorCode = program.build("-cl-std=CL1.0");																//Cycle through all the verisons of opencl and build accordingly
	if (clCurrentVersion == OpenCLVersionv11) clErrorCode = program.build("-cl-std=CL1.1");
	if (clCurrentVersion == OpenCLVersionv12) clErrorCode = program.build("-cl-std=CL1.2");
	if (clCurrentVersion == OpenCLVersionv20) clErrorCode = program.build("-cl-std=CL2.0");
	if (clCurrentVersion == OpenCLVersionv22) clErrorCode = program.build("-cl-std=CL2.1");
	if (clCurrentVersion == OpenCLVersionv22) clErrorCode = program.build("-cl-std=CL2.2");

	if (clErrorCode < 0)																																//Check for errors
	{
		string errstring(getErrorString(clErrorCode));
		printf("\n       ERROR COULD COMPILE: %s", errstring.c_str());
		cl::STRING_CLASS errmsg = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clCurrentDevice);
		printf("\n%s", errmsg.c_str());
		return cl::Kernel();
	}

	cl::Kernel kernel(program, funcname.c_str(), &clErrorCode);																							//Create the kernel

	if (clErrorCode < 0)																																//Check for errors
	{
		string errstring(getErrorString(clErrorCode));
		printf("\n       ERROR COULD COMPILE: %s", errstring.c_str());
		printf("\n%s", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clCurrentDevice).c_str());
		return cl::Kernel();
	}

	printf("\nLoaded KERNEL %s %s", funcname.c_str(), filename.c_str());

	return kernel;
}

/*===NOTE===================================================================================*/
/* This is a simple way to execute a kernel without any hassle                
cl_int clExecuteKernel(cl::Kernel kernel)
{
	clCurrentCommandQue.enqueueTask(kernel);

	return 0;
}

/*===NOTE===================================================================================*/
/* This function right here is not my code:
/* https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes
*/

const char* getErrorString(cl_int error)
{
	switch (error) {
		// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}

#endif