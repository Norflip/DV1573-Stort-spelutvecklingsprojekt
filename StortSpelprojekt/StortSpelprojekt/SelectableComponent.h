#include "Object.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
namespace dx = DirectX;

class SelectableComponent :public Component
{
private:
	
	bool isActive;

public:
	SelectableComponent();
	virtual ~SelectableComponent();
	void SetActive(bool isActive);
	bool GetActive() { return this->isActive; }
};