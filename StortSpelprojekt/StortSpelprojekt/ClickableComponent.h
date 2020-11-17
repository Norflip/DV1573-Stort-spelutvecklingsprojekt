#include "Object.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
namespace dx = DirectX;


enum class Type { Switch };

class PickupComponent :public Component
{
private:
	float value;
	Type clickableType;
	bool isActive;

public:

	PickupComponent(Type type, float amount = 25.f);
	virtual ~PickupComponent();
	void SetActive(bool isActive);
	float GetAmount() { return this->value; }
	Type GetType() { return this->clickableType; }
	void Update(const float& deltaTime);
};