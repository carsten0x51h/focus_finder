/*
 * customtoolbutton.h
 *
 *  Created on: Apr 11, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_MODE_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_MODE_H_

/**
 * ImageViewerModeT
 */
struct ImageViewerModeT {
  enum TypeE {
    NAVIGATION, // Panning & zooming
    ROI_SELECT, // Region of interest selection mode
	POI_SELECT, // Point of interest selection mode (e.g. focusing star)
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case NAVIGATION: return "NAVIGATION";
    	case ROI_SELECT: return "ROI_SELECT";
    	case POI_SELECT: return "POI_SELECT";
    	default: return "<?>";
    }
  }
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_MODE_H_*/

