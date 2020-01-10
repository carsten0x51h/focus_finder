/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_

#include <vector>
#include "point.h"

///////////////////////////////////////////////////////////////////////
// Helper classes
///////////////////////////////////////////////////////////////////////
struct GslMultiFitDataT {
  //GslMultiFitDataT() : y(0.0f), sigma(0.0f) {}
GslMultiFitDataT(float y, float sigma, PointFT pt) : y(y), sigma(sigma), pt(pt) { }
	float y;
	float sigma;
	PointFT pt;
};

typedef std::vector<GslMultiFitDataT> GslMultiFitParmsT;


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_*/
