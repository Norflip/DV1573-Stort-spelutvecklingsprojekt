#include "GUIManager.h"

GUIManager::GUIManager(Renderer* renderer): renderer(renderer)
{
	spriteBatch = new DirectX::SpriteBatch(this->renderer->GetContext());
}

void GUIManager::AddGUIObject(GUIObject* addObj, std::string name)
{
	GUIObjects.insert({ name,addObj });
}

void GUIManager::AddGUIObject(GUIObject* addObj, std::string name, float x, float y)
{
}




void GUIManager::AddGUIObject(std::string textureName, std::string name, float x, float y)
{
}

GUIObject* GUIManager::GetGUIObject(std::string name)
{
	std::unordered_map<std::string, GUIObject*>::const_iterator got = GUIObjects.find(name);
	if (got == GUIObjects.end())
		return nullptr;
	else
		return got->second;
}

void GUIManager::RemoveGUIObject(std::string name)
{
	auto temp = GUIObjects.find(name);
	if (temp != GUIObjects.end())
		GUIObjects.erase(temp);
}

void GUIManager::DrawAll()
{
	spriteBatch->Begin();
	for (auto i : GUIObjects)
		i.second->Draw(spriteBatch);
	spriteBatch->End();
}