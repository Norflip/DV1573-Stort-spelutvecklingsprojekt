#include "Object.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
namespace dx = DirectX;


enum class ClickType { Switch };

class ClickableComponent :public Component
{
private:
	ClickType clickableType;
	bool isActive;

public:
	ClickableComponent(ClickType type);
	virtual ~ClickableComponent();
	void SetActive(bool isActive);
	bool GetActive() { return this->isActive; }
	ClickType GetType() { return this->clickableType; }
	void Update(const float& deltaTime);
};