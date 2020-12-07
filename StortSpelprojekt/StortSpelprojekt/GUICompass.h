#pragma once
#include "GUIManager.h"
#include "GUISprite.h"
#include "PlayerComp.h"
#include "ControllerComp.h"
#include <math.h> 
#include <cmath>
class GUIManager;

ALIGN16
class GUICompass : public GUIObject
{
public:
    GUICompass(Renderer& renderer, Window* window, Object* houseObj, Object* playerObj);
    void Update() override;
    virtual void Draw(DirectX::SpriteBatch*) override;
    float GetDistance()const { return distance; }
    virtual void SetPosition(float x, float y) override;
    GUISprite* GetBarSprite() { return this->backgroundBar; }
    GUISprite* GetHouseSprite() { return this->house; }
    ALIGN16_ALLOC;
private:
    Object* playerObj;
    Object* houseObj;
    GUISprite* backgroundBar;
    GUISprite* house;
    GUISprite* fuelTest;;
    Renderer* renderer;
    Window* window;
    float distance;
    float compassYpos;
    float compassXpos;
    sm::Vector3 playerPos;
    sm::Vector3 housePos;
    CameraComponent* cam;
};

