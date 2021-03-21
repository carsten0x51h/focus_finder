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

#include "include/curve_parm.h"

#include <fstream>
#include <string>

CurveParmT::CurveParmT() :
        mName(""), mValue(0.0F) {
}

CurveParmT::CurveParmT(const std::string &name, float value = 0.0F) :
        mName(name), mValue(value) {
}

const std::string &CurveParmT::getName() const {
    return mName;
}

float CurveParmT::getValue() const {
    return mValue;
}

void CurveParmT::setValue(float value) {
    mValue = value;
}

std::ostream &
CurveParmT::print(std::ostream &os) const {
    os << mName << " = " << mValue;
    return os;
}

std::ostream &operator<<(std::ostream &os, const CurveParmT &curveParm) {
    return curveParm.print(os);
}
