/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include "include/star_details.h"

StarDetailsT::StarDetailsT() : mMin(0.0F), mMax(0.0F), mHfd(0.0F), mFwhmHorz(0.0F), mFwhmVert(0.0F), mFwhmAvg(0.0F) {
}

float StarDetailsT::getMin() const {
    return mMin;
}

void StarDetailsT::setMin(float min) {
    StarDetailsT::mMin = min;
}

float StarDetailsT::getMax() const {
    return mMax;
}

void StarDetailsT::setMax(float max) {
    StarDetailsT::mMax = max;
}

float StarDetailsT::getHfd() const {
    return mHfd;
}

void StarDetailsT::setHfd(float hfd) {
    StarDetailsT::mHfd = hfd;
}

float StarDetailsT::getFwhmHorz() const {
    return mFwhmHorz;
}

void StarDetailsT::setFwhmHorz(float fwhmHorz) {
    StarDetailsT::mFwhmHorz = fwhmHorz;
}

float StarDetailsT::getFwhmVert() const {
    return mFwhmVert;
}

void StarDetailsT::setFwhmVert(float fwhmVert) {
    StarDetailsT::mFwhmVert = fwhmVert;
}

float StarDetailsT::getFwhmAvg() const {
    return mFwhmAvg;
}

void StarDetailsT::setFwhmAvg(float fwhmAvg) {
    StarDetailsT::mFwhmAvg = fwhmAvg;
}
