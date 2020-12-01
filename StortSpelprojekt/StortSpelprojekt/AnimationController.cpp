#include "stdafx.h"
#include "AnimationController.h"

bool AnimationController::Initialize()
{
    
    return false;
}

bool AnimationController::AdvanceTime(double deltaTime)
{
    for (size_t track = 0; track < _animTracks.size(); track++)
    {
        /*
              Check the status of all tracks 
			  animation sets are rendered only when the associated track is enabled 
			  Also check that the animation set associated with the track is "valid" 
        */
        if (!init)
        {
            return false;
        }

        if(_animTracks[track].desc.enable && _animTracks[track].animSet < (int)_animSets.size())
        {
            UINT animSetNum = _animTracks[track].animSet;
            _animSets[animSetNum].currentTime += deltaTime;

            if (!SetCurTicks(animSetNum))
            {
                return false;
            }

            //  loop through animations 
            for (size_t i = 0; i < _animSets[animSetNum].animations.size(); i++)
            {
                if (!InterpolateAnimations(_animSets[animSetNum].animations[i], _animSets[animSetNum].currentTime, _animTracks[track].trackKeys))
                {
                    return false;
                }
            }
        }

        dx::SimpleMath::Matrix rot, scale, translate;

        if (_trackEvents.size())
        {
            _trackEvents[0].curTime += deltaTime;
            if (_trackEvents[0].curTime > _trackEvents[0].transitionTime)
            {
                SetTrackEnable(_trackEvents[0].fromTrack, false);
                _trackEvents.clear();
            }
            
            else
            {
             
               std::vector<BlendEvent>::iterator iter = _trackEvents.begin();
               float blendFactor = float(iter->curTime / iter->transitionTime);
               std::vector<AnimationKey>& vFrom = _animTracks[iter->fromTrack].trackKeys;
               std::vector<AnimationKey>& vTo = _animTracks[iter->toTrack].trackKeys;

               dx::SimpleMath::Quaternion quatFinal, quatFrom, quatTo;
               dx::SimpleMath::Vector3 scaleFinal, scaleFrom, scaleTo, translateFinal, translateFrom, translateTo;

               for (size_t k = 0; k < vFrom.size(); k++)
               {
                   quatFrom = vFrom[k].rotationQuaternion;
                   quatTo = vTo[k].rotationQuaternion;
                   scaleFrom = vFrom[k].scaleVector;
                   scaleTo = vTo[k].scaleVector;
                   translateFrom = vFrom[k].translationVector;
                   translateTo = vTo[k].translationVector;

                   dx::XMQuaternionRotationMatrix(rot);



                   quatFinal = dx::SimpleMath::Quaternion::Slerp(quatFrom,quatTo, blendFactor);
                   scaleFinal = (1.0f - blendFactor) * scaleFrom + blendFactor * scaleTo;
                   translateFinal = (1.0f - blendFactor) * translateFrom + blendFactor * translateTo;

                   rot = rot.CreateFromQuaternion(quatFinal);
                   scale = scale.CreateTranslation(scaleFinal);
                   translate = translate.CreateTranslation(translateFinal);

                   //Osäker på denna sektion VIKTOR
                   Frame* frame;
                   frame->frameName = skeletonAni.GetKeyFrames()[k][0].name;

                   if (frame == NULL) return false;

                   frame->frameMatrix = rot * scale * translate;
                   
                 //VIKTOR
               }
            }
        }

        else
        {
            for (size_t track2 = 0; track2 < _animTracks.size(); track2++)
            {
                if (_animTracks[track2].desc.enable)
                {
                    std::vector<AnimationKey>& keys = _animTracks[track2].trackKeys;
                    for (size_t tk = 0; tk < keys.size(); tk++)
                    {
                        rot = rot.CreateFromQuaternion(keys[tk].rotationQuaternion);
                        scale = scale.CreateTranslation(keys[tk].scaleVector);
                        translate = translate.CreateTranslation(keys[tk].translationVector);

                        //Osäker på denna sektion VIKTOR
                        Frame* frame;
                        frame->frameName = skeletonAni.GetKeyFrames()[tk][0].name;

                        if (frame == NULL) return false;

                        frame->frameMatrix = rot * scale * translate;

                    }
                }
            }

            return true;
        }
    }
}

bool AnimationController::TransitionTracks(int fromTrack, int toTrack, float transitionTime)
{
    if (fromTrack >= _animTracks.size() ||
        toTrack >= _animTracks.size() ||
        fromTrack == toTrack ||
        transitionTime < 0.0f)
    {
        return false;
    }

    BlendEvent blend;
    blend.fromTrack = fromTrack;
    blend.toTrack = toTrack;
    blend.transitionTime = transitionTime;
    blend.curTime = 0.0f;
    _trackEvents.push_back(blend);
    SetTrackEnable(fromTrack, true);
    SetTrackEnable(toTrack, true);
    return true;
}

bool AnimationController::BuildFrameNameIndex()
{
    frameNames.clear();

    frameNames.push_back(_rootFrame.rootFrame->frameName);
     //OSÄKER AF VIKTOR
    for (size_t animSet = 0; animSet < _animSets.size(); animSet++)
    {
        for (size_t anim = 0; anim < _animSets[animSet].animations.size(); anim++)
        {
            // VÄLDIGT OSÄKER
            if ((_animSets[animSet].animations[anim].keyIndex =
                IndexForFrameName(_animSets[animSet].animations[anim].frameName)) < 0)
            {
                return false;
            }
        }
        return true;
    }
    
}

bool AnimationController::SetCurTicks(size_t animSetNum)
{

    if (animSetNum >= _animSets.size())
    {
        return false;
    }
    float curTime = _animSets[animSetNum].currentTime;
    float period = _animSets[animSetNum].period;

    while (curTime >= period) curTime -= period;
    {
        _animSets[animSetNum].currentTime = curTime;
    }

    return true;
}

bool AnimationController::InterpolateAnimations(Animation& anim, double fTime, std::vector<AnimationKey>& trackKeys)
{

    size_t i = 0;
    dx::SimpleMath::Quaternion quat;
    dx::SimpleMath::Vector3 scale;
    dx::SimpleMath::Vector3 trans;


    if (anim.animKeys.size() > 1)
    {
        //Osäker VIKTOR
        anim.animKeys[i].time >= fTime;
        
    }

    while (i < anim.animKeys.size() && anim.animKeys[i].time < fTime)
    {
        i++;
    }

    if (i >= anim.animKeys.size())
    {
        i = anim.animKeys.size() - 1;

        fTime = anim.animKeys[i].time;
    }

    if (anim.animKeys[i].time >= fTime) //Osäker VIKTOR
    {
        if (i > 0)
        {
            float ratio = (fTime - anim.animKeys[i - 1].time) / (anim.animKeys[i - 1].time);
            // Osäker VIKTOR
            quat = dx::SimpleMath::Quaternion::Slerp(anim.animKeys[1 - i].rotationQuaternion, anim.animKeys[i].rotationQuaternion, ratio);
            trackKeys[anim.keyIndex].rotationQuaternion = quat;
            trackKeys[anim.keyIndex].scaleVector = scale;
            trackKeys[anim.keyIndex].translationVector = trans;


        }

        else
        {
            trackKeys[anim.keyIndex].rotationQuaternion = anim.animKeys[0].rotationQuaternion;
            trackKeys[anim.keyIndex].scaleVector = anim.animKeys[0].scaleVector;
            trackKeys[anim.keyIndex].translationVector = anim.animKeys[0].translationVector;
        }
    }
    return true;
}

bool AnimationController::InterpolateKeyFrame(AnimationKey& animKey, double fTime)
{
    return false;
}

void AnimationController::AddFrameName(Frame* frame)
{
    frameNames.push_back(std::string(frame->frameName));

    // gör så att ag rekursivt kan hitta alla children. 
    //KOLLA SKSLETONANI HUR VI GJORDE DÄR
    for (int i = 0; i < skeletonAni.GetKeyFrames().size(); i++)
    {
        AddFrameName();
    }
}

int AnimationController::IndexForFrameName(std::string frameName)
{
    for (size_t i = 0; i < frameNames.size(); i++)
    {
        if (frameNames[i] == frameName)
        {
            return i;
        }
        return -1;
    }
}
