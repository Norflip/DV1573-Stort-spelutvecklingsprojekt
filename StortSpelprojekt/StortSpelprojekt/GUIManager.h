#pragma once
#include "DirectXHelpers.h"
#include <SpriteBatch.h>
#include "Renderer.h"
namespace dx = DirectX;

class GUIManager
{
public:
	GUIManager(Renderer* renderer);
private:
	dx::SpriteBatch* spriteBatch;
	Renderer* renderer;
};

