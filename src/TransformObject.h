#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

// Elston Ma - CS134 - Final Project

//  Base class for any object that needs a transform.
// editted by Elston Ma for CS134 Final Project
class TransformObject {
public:
	TransformObject();
	ofVec3f position, scale;
	float	rotation;
	bool	bSelected;
	void setPosition(const ofVec3f &);
};