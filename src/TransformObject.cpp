#pragma once
#include "TransformObject.h"

// From Kevin Smith CS134 SJSU

// Elston Ma - CS134 - Final Project

//  Base class for any object that needs a transform.
//
TransformObject::TransformObject() {
	position = ofVec3f(0, 0, 0);
	scale = ofVec3f(1, 1, 1);
	rotation = 0;
}

void TransformObject::setPosition(const ofVec3f & pos) {
	position = pos;
}
