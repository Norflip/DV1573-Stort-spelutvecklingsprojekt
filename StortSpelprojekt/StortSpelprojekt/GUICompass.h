#pragma once
#include "GUIManager.h"
#include "GUISprite.h"
#include "PlayerComp.h"
#include "ControllerComp.h"
#include <math.h> 
#include <cmath>
class GUIManager;

class GUICompass : public GUIObject
{
public:
    GUICompass(Renderer& renderer, Window* window, Object* houseObj, Object* playerObj);
    void Update() override;
    virtual void Draw(DirectX::SpriteBatch*) override;

    virtual void SetPosition(float x, float y) override; // the whole compass shouldnt change pos
private:
    Object* playerObj;
    Object* houseObj;
    GUISprite* backgroundBar;
    GUISprite* house;
    GUISprite* fuelTest;;
    Renderer* renderer;
    Window* window;
    float compassYpos;
    float compassXpos;
};

