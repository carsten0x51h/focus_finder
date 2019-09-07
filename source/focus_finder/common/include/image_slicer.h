#ifndef _IMAGE_SLICER_H_
#define _IMAGE_SLICER_H_ _IMAGE_SLICER_H_

#include <vector>

#include "logging.h"
#include "exception.h"
#include "image.h"
#include "enum_helper.h"

/**
 * NOTE: Old code which was used for old DirectionT (now SliceDirectionT)
 *       to be passed from cmdline (which now is probably not required any
 *       more).
 */
//#include <boost/algorithm/string/case_conv.hpp>
// DirectionT validator for boost cmd line parser
//void validate(boost::any & v, const vector<string> & values,
//		DirectionT::TypeE * target_type, int) {
//	using namespace boost::program_options;
//
//	validators::check_first_occurrence(v);
//	string s = validators::get_single_string(values);
//	boost::to_upper(s);
//	DirectionT::TypeE type = DirectionT::asType(s.c_str());
//
//	if (type != DirectionT::_Count) {
//		v = any(type);
//	} else {
//		throw validation_error(validation_error::invalid_option_value);
//	}
//}


/**
 * Helper class - horizontal and vertical.
 */
struct SliceDirectionT {
	enum TypeE {
		HORZ, VERT, _Count
	};

	static const char * asStr(const TypeE & inType) {
		switch (inType) {
		case HORZ:
			return "HORZ";
		case VERT:
			return "VERT";
		default:
			return "<?>";
		}
	}

	MAC_AS_TYPE(Type, E, _Count);
};

DEF_Exception(ImageSlicer);

class ImageSlicerT {
public:
	template<SliceDirectionT::TypeE D> static std::vector<typename ImageT::value_type> slice(
			const ImageT & inImage, size_t index) {
		std::vector<typename ImageT::value_type> values;

		// Extract slices through centroid for profiles
		switch (D) {
		case SliceDirectionT::HORZ: {
			values.resize(inImage.width());

			cimg_forX(inImage, x)
			{
				values[x] = inImage(x, index);
			}
			break;
		}
		case SliceDirectionT::VERT: {
			values.resize(inImage.height());

			cimg_forY(inImage, y)
			{
				values[y] = inImage(index, y);
			}
			break;
		}
		default: {
			throw ImageSlicerExceptionT("Invalid direction.");
		}
		}
		return values;
	}
};

#endif /*_IMAGE_SLICER_H_*/
