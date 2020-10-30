#pragma once

#define win32_lean_and_mean
//GLOBAL DEFINES

#define ALIGN16 __declspec(align(16))
#define RELEASE(cptr) if(cptr) { cptr->Release(); cptr = nullptr; }

#define FCAST(T) static_cast<float>(T)
#define DCAST(T) static_cast<double>(T)
#define ICAST(T) static_cast<int>(T)
#define UICAST(T) static_cast<unsigned int>(T)

#define MM_MALLOC_OPERATOR void* operator new(size_t i) { return _mm_malloc(i, 16); }
#define MM_FREE_OPERATOR void operator delete(void* p){ _mm_free(p); }
#define ALIGN16_ALLOC MM_MALLOC_OPERATOR MM_FREE_OPERATOR
//GLOBAL INCLUDES
#include <react3d.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h> 
#include <ImportZWEB.h>
#include <DirectXMath.h>
#include <map>
#include <SimpleMath.h>
#include <functional>
#include <queue>
#include <random>
#include <assert.h>
#include <bitset>
#include <array>
#include <algorithm>

//GLOBAL HEADERS
#include "Random.h"
#include "Physics.h"
#include "Window.h"
#include "Input.h"
#include "Message.h"
#include "MessageBus.h"
#include "Log.h"
#include "GameClock.h"
#include "ZWEBLoader.h"
#include "DirectXHelpers.h"
//GLOBAL NAMESPACES
namespace dx = DirectX;

