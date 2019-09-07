/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_STAR_INFO_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_STAR_INFO_H_

#include "point.h"

class StarInfoT {
public:
	const PointT<float> & getCenter() const;
	void setCenter(const PointT<float> & center);

	void setValidStarDetected(bool validStarDetected);
	bool isValidStarDetected() const;


private:
	PointT<float> mCenter;		// Position of the center - in principle can contain coordinates of all coordinate systems
	bool mValidStarDetected;	// Was a valid star detected with center a mCenterPos?
	// HfdT
	// SNR
	// FwhmT fwhmHorz;
	// FwhmT fwhmVert;
	// ImageT??
};



#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_STAR_INFO_H_ */
