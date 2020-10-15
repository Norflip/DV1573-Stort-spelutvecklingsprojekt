#pragma once
#include "Object.h"

class AIIdle : public Component
{
	public:
		AIIdle();
		virtual ~AIIdle();
		void Update(const float& deltaTime) override;
	private:
};