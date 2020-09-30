#include "GUIManager.h"

GUIManager::GUIManager(Renderer* renderer): renderer(renderer)
{
	spriteBatch = new DirectX::SpriteBatch(this->renderer->GetContext());
}
