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

#include "include/indi_helper.h"

std::string IndiHelperT::propPermAsStr(IPerm perm) {
    switch (perm) {
        case IP_RO:
            return "IP_RO";
        case IP_WO:
            return "IP_WO";
        case IP_RW:
            return "IP_RW";
        default:
            return "<?>";
    }
}

std::string IndiHelperT::propStateAsStr(IPState state) {
    switch (state) {
        case IPS_IDLE:
            return "IPS_IDLE";
        case IPS_OK:
            return "IPS_OK";
        case IPS_BUSY:
            return "IPS_BUSY";
        case IPS_ALERT:
            return "IPS_ALERT";
        default:
            return "<?>";
    }
}

INumberVectorProperty *IndiHelperT::getNumberVec(INDI::BaseDevice *dev, const std::string &vecPropName) {

    INumberVectorProperty *vecProp = dev->getNumber(vecPropName.c_str());

    if (vecProp == nullptr) {
        throw IndiExceptionT("Number vector '" + vecPropName + "' not found.");
    }

    return vecProp;
}

INumber *IndiHelperT::getNumber(INumberVectorProperty *vec, const std::string &propName) {

    INumber *prop = IUFindNumber(vec, propName.c_str());

    if (prop == nullptr) {
        throw IndiExceptionT("Number '" + propName + "' not found.");
    }

    return prop;
}


ISwitchVectorProperty *IndiHelperT::getSwitchVec(INDI::BaseDevice *dev, const std::string &vecPropName) {

    ISwitchVectorProperty *vecProp = dev->getSwitch(vecPropName.c_str());

    if (vecProp == nullptr) {
        throw IndiExceptionT("Switch vector '" + vecPropName + "' not found.");
    }

    return vecProp;
}

ISwitch *IndiHelperT::getSwitch(ISwitchVectorProperty *vec, const std::string &propName) {

    ISwitch *prop = IUFindSwitch(vec, propName.c_str());

    if (prop == nullptr) {
        throw IndiExceptionT("Switch '" + propName + "' not found.");
    }

    return prop;
}

ITextVectorProperty *IndiHelperT::getTextVec(INDI::BaseDevice *dev, const std::string &vecPropName) {

    ITextVectorProperty *vecProp = dev->getText(vecPropName.c_str());

    if (vecProp == nullptr) {
        throw IndiExceptionT("Text vector '" + vecPropName + "' not found.");
    }

    return vecProp;
}

IText *IndiHelperT::getText(ITextVectorProperty *vec, const std::string &propName) {

    IText *prop = IUFindText(vec, propName.c_str());

    if (prop == nullptr) {
        throw IndiExceptionT("Text '" + propName + "' not found.");
    }

    return prop;
}
