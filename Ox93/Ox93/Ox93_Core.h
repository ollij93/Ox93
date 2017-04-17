#pragma once

#ifndef __OX93_CORE_H__
#define __OX93_CORE_H__

/////////////////////////////////////////
// T Y P E D E F S
/////////////////////////////////////////

typedef unsigned int u_int;
typedef unsigned long u_long;
typedef u_int Ox93_Hash;

struct Ox93_Color
{
	float fRed;
	float fBlue;
	float fGreen;
	float fAlpha;
};

typedef void(*VoidFunction)();
typedef void*(*PopulationFunction)(u_int);
typedef void(*ListFunction)(int);

/////////////////////////////////////////
// Windows Includes
/////////////////////////////////////////
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "Winmm.lib")

/////////////////////////////////////////
// Direct X Includes
/////////////////////////////////////////
#include <dxgi.h>
#include <dxgidebug.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DirectX/DDSTextureLoader/DDSTextureLoader.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
/////////////////////////////////////////

/////////////////////////////////////////
// Ox93 Includes
/////////////////////////////////////////
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Specification/Ox93_Specification_Hash_IDs.h"
#include "Types/Ox93_Hash.h"

/////////////////////////////////////////
// Other Includes
/////////////////////////////////////////
#include <list>
#include <vector>


#endif // ifndef __OX93_CORE_H__