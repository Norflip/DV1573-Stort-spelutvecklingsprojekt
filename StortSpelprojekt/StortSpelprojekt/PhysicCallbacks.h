#pragma once
#include "Object.h"
#include "RigidBodyComponent.h"
#include "Ray.h"
#include <react3d.h>

class RaycastSingleResultCallback : public rp::RaycastCallback {

public:
	RayHit& hit;

	RaycastSingleResultCallback(RayHit& hit) : hit(hit)
	{
	//	hit.body = nullptr;
		hit.object = nullptr;
	}

	virtual float notifyRaycastHit(const rp::RaycastInfo& info)
	{
		std::cout << "Hit point : " << info.worldPoint.x << info.worldPoint.y << info.worldPoint.z << std::endl;

		hit.didHit = true;
		hit.position = dx::XMFLOAT3(info.worldPoint.x, info.worldPoint.y, info.worldPoint.z);
		hit.normal = dx::XMFLOAT3(info.worldNormal.x, info.worldNormal.y, info.worldNormal.z);
		hit.object = static_cast<Object*>(info.body->getUserData());
	//	hit.body = hit.object->GetComponent<RigidBodyComp>();

		// Return a fraction of 1.0 to gather all hits 
		return 0.0;
	}
};


// Your event listener class 
class CollisionEventListener : public rp::EventListener 
{
    virtual void onContact(const CollisionCallback::CallbackData& callbackData) override 
	{
        for (size_t p = 0; p < callbackData.getNbContactPairs(); p++) 
		{
			rp::CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

            for (size_t c = 0; c < contactPair.getNbContactPoints(); c++) {

				Object* object0 = static_cast<Object*>(contactPair.getBody1()->getUserData());
				Object* object1 = static_cast<Object*>(contactPair.getBody2()->getUserData());

				/*if (object0 != nullptr && object1 != nullptr)
				{
					CollisionInfo info;
					
					RigidBodyComp* body0 = object0->GetComponent<RigidBodyComp>();
					if (body0 != nullptr && body0->IsDynamic())
					{
						info.other = object1;
						body0->m_OnCollision(info);
					}

					RigidBodyComp* body1 = object1->GetComponent<RigidBodyComp>();
					if (body1 != nullptr && body1->IsDynamic())
					{
						info.other = object0;
						body1->m_OnCollision(info);
					}
				}*/
            }
        }
    }
};
