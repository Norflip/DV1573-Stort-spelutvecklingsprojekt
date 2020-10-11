#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position, dx::XMFLOAT4 quaterionRotation)
    : extends(extends), position(position), rotation(quaterionRotation)
{

}

void BoxColliderComponent::Initialize()
{
    shape = new btBoxShape(btVector3(extends.x, extends.y, extends.z));

   // btQuaternion btRotation (rotation.x, rotation.y, rotation.z, rotation.w);
    btVector3 btPosition (position.x, position.y, position.z);
   // transform = btTransform(btRotation, btPosition);
    transform.setIdentity();
    transform.setOrigin(btPosition);


}