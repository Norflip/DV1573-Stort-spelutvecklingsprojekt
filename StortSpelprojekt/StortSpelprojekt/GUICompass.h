#pragma once
#include "GUIManager.h"
#include "GUISprite.h"
class GUIManager;

class GUICompass : public GUIObject
{
public:
    GUICompass(Renderer& renderer, Window* window);
    void Update() override;
    virtual void Draw(DirectX::SpriteBatch*) override;

    virtual void SetPosition(float x, float y) override;
private:
    GUISprite* backgroundBar;
    GUISprite* house;
    GUISprite* fuelTest;;
    Renderer* renderer;
    Window* window;
};

