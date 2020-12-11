//#pragma once
//#include "SkeletonAni.h"
//#include <iostream>
//#include <algorithm>
//#include <iterator>
//
//#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
//
//struct Frame
//{
//	unsigned int frameindex;
//	std::string frameName;
//	dx::SimpleMath::Matrix frameMatrix;
//};
//
//struct AnimationKey
//{
//	Frame* frame;
//	float time;
//	//std::string parentName;
//	dx::SimpleMath::Quaternion rotationQuaternion;
//	dx::SimpleMath::Vector3 translationVector;
//	dx::SimpleMath::Vector3 scaleVector {1, 1, 1};
//};
//
//struct Animation
//{
//	std::string frameName;
//	std::string boneName;
//	unsigned int keyIndex;
//	float maxTime;
//	std::vector<AnimationKey> animKeys;
//};
//
//struct AnimationSet
//{
//	bool checkRootFrame;
//	std::string animationSetname;
//	std::vector<Animation> animations;
//	double currentTime, period;
//};
//
//struct RootFrame
//{
//	bool checkAnimationSets;
//	Frame* rootFrame;
//};
//
//struct AnimTrackDesc
//{
//	float speed;
//	bool enable;
//};
//
//struct BlendEvent
//{
//	int fromTrack, toTrack;
//	float transitionTime;
//	float curTime;
//};
//
//struct AnimTrack
//{
//	int animSet;
//	std::vector<AnimationKey> trackKeys;
//	std::vector<Frame>& rootFrame;
//	AnimTrackDesc desc;
//};
//
//class AnimationController
//{
//public:
//	AnimationController() { init = false; }
//
//	bool Initialize();
//	bool AdvanceTime(double deltaTime);
//	bool SetHierarchy(Animation* newRootFrame);
//	bool SetTicksPerSecond(int newTicks, int whichAnimationSet);
//	size_t GetNumAnimationSets() { return _animSets.size(); }
//
//	bool GetAnimationSetName(int animSetNum, std::string& animName);
//	size_t GetNumTracks() { return _animTracks.size(); }
//	bool TrackSetAnimationSet(int animSetNum, int trackNum);
//	bool TrackSetAnimationSet(std::string animSetName, int trackNum);
//	int CreateTrack(AnimTrackDesc* trackDesc);											// returns the index for the new track 
//	bool DeleteTrack(int trackNum);
//	bool GetTrackDescription(int trackNum, AnimTrackDesc* trackDesc);
//	bool SetTrackDescription(int trackNum, AnimTrackDesc* trackDesc);
//	bool GetTrackEnable(int trackNum);
//	bool SetTrackEnable(int trackNum, bool enable);
//	bool TransitionTracks(int fromTrack, int toTrack, float transitionTime);				// start a blend event 
//	size_t GetNumBlendEvents() { return _trackEvents.size(); }
//	bool GetBlendEvent(int eventNum, BlendEvent* event);
//
//	bool CheckCompatibility();					 //are the animation sets applicable to the frame hierarchy?
//	bool BuildFrameNameIndex();					 //sets Animation::trackKeyIndex 
//	bool SetCurTicks(size_t animSetNum);
//	bool InterpolateAnimations(Animation& anim, double fTime, std::vector<AnimationKey>& trackKeys);
//	bool InterpolateKeyFrame(AnimationKey& animKey, double fTime);
//
//	void AddFrameName(Frame* frame);
//	int IndexForFrameName(std::string frameName);
//
//	SkeletonAni skeletonAni;
//	RootFrame _rootFrame;				// frame hierarchy access (for storing matrices, finding names, etc).
//	std::vector<AnimationSet> _animSets;							// all the animation sets available
//	std::vector<AnimTrack> _animTracks;						// all the tracks
//	std::vector<BlendEvent> _trackEvents;						// information for blending animation sets 
//	std::vector<std::string> frameNames;							// this of hierarchy frame names used to index into track
//	bool init;
//
//private:
//
//};