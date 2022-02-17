#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

 // Elston Ma - CS134 - Final Project

class Box {
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) {
 //     assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;

    // corners
    Vector3 parameters[2];

	Vector3 min() const { return parameters[0]; }
	Vector3 max() const { return parameters[1]; }
	const bool inside(const Vector3 &p) const {
		return ((p.x() >= parameters[0].x() && p.x() <= parameters[1].x()) &&
		     	(p.y() >= parameters[0].y() && p.y() <= parameters[1].y()) &&
			    (p.z() >= parameters[0].z() && p.z() <= parameters[1].z()));
	}
	const bool inside(Vector3 *points, int size) {
		bool allInside = true;
		for (int i = 0; i < size; i++) {
			if (!inside(points[i])) allInside = false;
			break;
		}
		return allInside;
	}

	// implement for Homework Project
	// developed by Elston Ma for CS134 Project 2
	 bool overlap(const Box &box) const {
		 // check this length: (this->center() - box.center()).length()
		 // threshold length: (this->center() - this->min()).length() 
		 // + (box.center() - box.min()).length()
		 //Box copyBox = box;

		 // maybe check if points of box are inside this->box?

		 
		 //float currentDistance = (this->center() - box.center()).length();
		 /*
		 float threshold = (this->center() - this->min()).length() + 
			 (box.center() - box.min()).length();
		 return currentDistance <= threshold;
		 */

		 // the 8 corners of this Box
		 Vector3 thisPt1(this->min().x(), this->min().y(), this->min().z());
		 Vector3 thisPt2(this->min().x(), this->min().y(), this->max().z());
		 Vector3 thisPt3(this->min().x(), this->max().y(), this->min().z());
		 Vector3 thisPt4(this->min().x(), this->max().y(), this->max().z());
		 Vector3 thisPt5(this->max().x(), this->min().y(), this->min().z());
		 Vector3 thisPt6(this->max().x(), this->min().y(), this->max().z());
		 Vector3 thisPt7(this->max().x(), this->max().y(), this->min().z());
		 Vector3 thisPt8(this->max().x(), this->max().y(), this->max().z());

		 // the 8 corners of argument box
		 Vector3 boxPt1(box.min().x(), box.min().y(), box.min().z());
		 Vector3 boxPt2(box.min().x(), box.min().y(), box.max().z());
		 Vector3 boxPt3(box.min().x(), box.max().y(), box.min().z());
		 Vector3 boxPt4(box.min().x(), box.max().y(), box.max().z());
		 Vector3 boxPt5(box.max().x(), box.min().y(), box.min().z());
		 Vector3 boxPt6(box.max().x(), box.min().y(), box.max().z());
		 Vector3 boxPt7(box.max().x(), box.max().y(), box.min().z());
		 Vector3 boxPt8(box.max().x(), box.max().y(), box.max().z());

		 return this->inside(boxPt1) || this->inside(boxPt2) || this->inside(boxPt3) || this->inside(boxPt4) ||
			 this->inside(boxPt5) || this->inside(boxPt6) || this->inside(boxPt7) || this->inside(boxPt8) ||
			 box.inside(thisPt1) || box.inside(thisPt2) || box.inside(thisPt3) || box.inside(thisPt4) ||
			 box.inside(thisPt5) || box.inside(thisPt6) || box.inside(thisPt7) || box.inside(thisPt8);

		 /*
		 return (((box.min().x() < this->min().x()) && (this->min().x() < box.max().x())) || 
			((box.min().x() < this->max().x()) && (this->max().x() < box.max().x()))) &&
			 (((box.min().y() < this->min().y()) && (this->min().y() < box.max().y())) ||
			 ((box.min().y() < this->max().y()) && (this->max().y() < box.max().y()))) &&
			(((box.min().z() < this->min().z()) && (this->min().z() < box.max().z())) ||
			 ((box.min().z() < this->max().z()) && (this->max().z() < box.max().z())));
		 */

		 //return box.inside(this->center());

		 //return true;
	}

	Vector3 center() const {
		return ((max() - min()) / 2 + min());
	}
};

#endif // _BOX_H_
