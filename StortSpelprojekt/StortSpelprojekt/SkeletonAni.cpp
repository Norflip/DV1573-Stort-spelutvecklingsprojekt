#include "SkeletonAni.h"

DirectX::XMFLOAT4X4& SkeletonAni::Lerp(float elapsedTime, std::vector<Bone>& keys)
{
    animationTime = elapsedTime * fps;
    //animationTime += elapsedTime;
    currentFrame = fmodf(animationTime, lenght);

    firstIndex = 0;
    secondIndex = 1;

    for (unsigned int i = 0; i < keys.size() - 1; i++) //Find what frames the timeline is currently in between.
    {

        if (currentFrame < keys[i + 1].frame)
        {
            firstIndex = i;
            secondIndex = i + 1;
            break;
        }


    }


    //lerp between the frames to find how close to each frame the animation is.

    t = (currentFrame - (float)firstIndex) / ((float)secondIndex - (float)firstIndex);

    CLAMP(t, 0, 1); //saturate the interpolation value


    if (t < 0.0f)
    {
        t = 0.0f;
    }
    else if (t > 1.0f)
    {
        t = 1.0f;
    }

    assert(t >= 0.0f && t <= 1.0f);

    
    dx::XMStoreFloat4(&rotQ, DirectX::XMQuaternionSlerp(dx::XMLoadFloat4(&keys[firstIndex].rotationQuaternion), dx::XMLoadFloat4(&keys[secondIndex].rotationQuaternion), t));
    
    dx::XMStoreFloat4x4(&rotQM, DirectX::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&rotQ)));
    
    dx::XMStoreFloat4(&transV, DirectX::XMVectorLerp(dx::XMLoadFloat4(&keys[firstIndex].translationVector), dx::XMLoadFloat4(&keys[secondIndex].translationVector), t));
    
    dx::XMStoreFloat4x4(&transM, DirectX::XMMatrixTranslationFromVector(dx::XMLoadFloat4(&transV)));

     

    dx::XMStoreFloat4x4(&RT, dx::XMLoadFloat4x4(&rotQM) * dx::XMLoadFloat4x4(&transM)); //I have omitted scale




    return RT;



}

SkeletonAni::SkeletonAni()
{
    animationTime = 0.0f;
    bones.resize(60); //Maximum number of bones, the animation doesn't need to have this many.
}

std::vector<dx::XMFLOAT4X4>& SkeletonAni::Makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys)
{
    DirectX::XMMATRIX toRoot = dx::XMLoadFloat4x4(&Lerp(elapsedTime, keys)) * globalParent; //These matrices are local, need to make them global recursively.



    unsigned int ftIndex = keys[0].index; //all of these indices have the same index number.

    DirectX::XMMATRIX finalTransform = dx::XMLoadFloat4x4(&offsetM[ftIndex]) * toRoot;
    
   

    XMStoreFloat4x4(&bones[ftIndex], DirectX::XMMatrixTranspose(finalTransform));
    

    for (unsigned int i = 0; i < keyBones.size(); i++) //recursively find all the children and repeat.
    {
        if (keyBones[i][0].parentName == keys[0].name)
        {
            Makeglobal(elapsedTime, toRoot, keyBones[i]);
        }
    }

    return bones;
    
}

std::string SkeletonAni::GetRootName()
{
    std::string rootName = "RootNode"; //This is the name given to each rootbone inside fbx sdk from me.
    std::string rootNode;

    for (unsigned int i = 0; i < keyBones.size(); i++)
    {
        if (keyBones[i][0].parentName == rootName)
        {
            rootNode = keyBones[i][0].name;
            break;
        }
    }

    return rootNode;
}

unsigned int SkeletonAni::GetNrOfBones() const
{
    return (unsigned int)offsetM.size(); //these are one per bone.
}

void SkeletonAni::SetUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets)
{
    offsetM.clear();
    offsetM.resize(offsets.size());
    for (unsigned int i = 0; i < offsets.size(); i++)
    {

        DirectX::XMMATRIX mat = DirectX::XMMatrixSet(offsets[i].m[0], offsets[i].m[1], offsets[i].m[2], offsets[i].m[3]
            , offsets[i].m[4], offsets[i].m[5], offsets[i].m[6], offsets[i].m[7]
            , offsets[i].m[8], offsets[i].m[9], offsets[i].m[10], offsets[i].m[11]
            , offsets[i].m[12], offsets[i].m[13], offsets[i].m[14], offsets[i].m[15]);

        dx::XMStoreFloat4x4(&offsetM[boneIDMap[(std::string)offsets[i].linkName]], mat);
        
    }
}

std::vector<Bone>& SkeletonAni::GetRootKeyJoints()
{
    for (unsigned int i = 0; i < keyBones.size(); i++)
    {
        if (keyBones[i][0].name == GetRootName())
        {
            return keyBones[i];
        }
    }
}

void SkeletonAni::SetUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght)
{
    this->fps = fps;
    this->lenght = aniLenght;
    this->boneIDMap = boneIDMap;


}

void SkeletonAni::SetUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys)
{
    std::vector<Bone> jointKeysVector;
    keyBones.resize(offsetM.size());
    for (unsigned int i = 0; i < keys.size(); i++)
    {
        Bone joint;

        joint.index = boneIDMap[(std::string)keys[i].linkName];
        joint.frame = keys[i].currentKeyFrameIndex;
        joint.name = (std::string)keys[i].linkName;
        joint.parentName = (std::string)keys[i].parentName;
        DirectX::XMVECTOR rot = DirectX::XMVectorSet(keys[i].r[0], keys[i].r[1], keys[i].r[2], keys[i].r[3]);
        DirectX::XMVECTOR trans = DirectX::XMVectorSet(keys[i].t[0], keys[i].t[1], keys[i].t[2], keys[i].t[3]); //konverteras till left handed i fbx


        dx::XMStoreFloat4(&joint.rotationQuaternion, rot);
        dx::XMStoreFloat4(&joint.translationVector, trans);
        


        jointKeysVector.push_back(joint); //every joint contains all the keyframes.



    }



    for (unsigned int i = 0; i < jointKeysVector.size(); i++)
    {
        keyBones[boneIDMap[boneName]].push_back(jointKeysVector[i]);
    }



}



std::map<std::string, unsigned int>& SkeletonAni::getBoneIDMap()
{
    return boneIDMap;
}


