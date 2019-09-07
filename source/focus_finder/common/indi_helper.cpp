#include "include/indi_helper.h"

std::string IndiHelperT::propPermAsStr(IPerm perm) {
	switch(perm) {
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
	switch(state) {
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

INumberVectorProperty * IndiHelperT::getNumberVec(INDI::BaseDevice * dev, const std::string & vecPropName) {

	INumberVectorProperty * vecProp = dev->getNumber(vecPropName.c_str());

	if (vecProp == nullptr) {
		throw IndiExceptionT("Number vector '" + vecPropName + "' not found.");
	}

	return vecProp;
}

INumber * IndiHelperT::getNumber(INumberVectorProperty * vec, const std::string & propName) {

	INumber * prop = IUFindNumber(vec, propName.c_str());

	if (prop == nullptr) {
		throw IndiExceptionT("Number '" + propName + "' not found.");
	}

	return prop;
}


ISwitchVectorProperty * IndiHelperT::getSwitchVec(INDI::BaseDevice * dev, const std::string & vecPropName) {

	ISwitchVectorProperty * vecProp = dev->getSwitch(vecPropName.c_str());

	if (vecProp == nullptr) {
		throw IndiExceptionT("Switch vector '" + vecPropName + "' not found.");
	}

	return vecProp;
}

ISwitch * IndiHelperT::getSwitch(ISwitchVectorProperty * vec, const std::string & propName) {

	ISwitch * prop = IUFindSwitch(vec, propName.c_str());

	if (prop == nullptr) {
		throw IndiExceptionT("Switch '" + propName + "' not found.");
	}

	return prop;
}

ITextVectorProperty * IndiHelperT::getTextVec(INDI::BaseDevice * dev, const std::string & vecPropName) {

	ITextVectorProperty * vecProp = dev->getText(vecPropName.c_str());

	if (vecProp == nullptr) {
		throw IndiExceptionT("Text vector '" + vecPropName + "' not found.");
	}

	return vecProp;
}

IText * IndiHelperT::getText(ITextVectorProperty * vec, const std::string & propName) {

	IText * prop = IUFindText(vec, propName.c_str());

	if (prop == nullptr) {
		throw IndiExceptionT("Text '" + propName + "' not found.");
	}

	return prop;
}
