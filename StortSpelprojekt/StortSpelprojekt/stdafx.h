#pragma once

#define win32_lean_and_mean

//Should never change
#include <react3d.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h> 

//New list, all objects here will be accessible as global
#include "Random.h"
#include "Physics.h"
#include "Window.h"
#include "Input.h"
#include "Message.h"
#include "MessageBus.h"
#include "Log.h"
#include "GameClock.h"
#include "ZWEBLoader.h"