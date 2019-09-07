/*
 * star_info.cpp
 *
 *  Created on: Apr 27, 2019
 *      Author: devnull
 */

#include "include/star_info.h"
#include "include/point.h"

const PointT<float> & StarInfoT::getCenter() const {
	return mCenter;
}

void StarInfoT::setCenter(const PointT<float> & center) {
	mCenter = center;
}

void StarInfoT::setValidStarDetected(bool validStarDetected) {
	mValidStarDetected = validStarDetected;
}

bool StarInfoT::isValidStarDetected() const {
	return mValidStarDetected;
}
