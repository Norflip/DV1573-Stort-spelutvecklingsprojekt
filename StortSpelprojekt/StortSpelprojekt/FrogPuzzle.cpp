#include "stdafx.h"
#include "FrogPuzzle.h"

FrogPuzzle::FrogPuzzle(ResourceManager* resourceManager)
{
	this->resource = resourceManager;
	this->frog = resource->AssembleObject("PuzzleFrogStatue", "PuzzleFrogStatueMaterial");
	this->fly = resource->AssembleObject("PuzzleFlyStatue", "PuzzleFlyStatueMaterial");

}

FrogPuzzle::~FrogPuzzle()
{
}

void FrogPuzzle::AddPuzzleElements(Object* frog, Object* fly)
{
	this->frog = frog;
	this->fly = fly;
}
