//
// Created by devnull on 26.02.22.
//

#ifndef FOFI_NEW_FRAME_EVENT_H
#define FOFI_NEW_FRAME_EVENT_H

#include <memory>
#include <utility>

#include "../image.h"
#include "../rect.h"

class NewFrameEventT {
public:
    NewFrameEventT(const RectT<unsigned int> & roiRect, std::shared_ptr<const ImageT> resultImage, bool lastExposure) : mRoiRect(roiRect), mResultImage(std::move(resultImage)), mLastExposure(lastExposure) {}

    [[nodiscard]] const RectT<unsigned int> &getRoiRect() const {
        return mRoiRect;
    }

    void setRoiRect(const RectT<unsigned int> &roiRect) {
        NewFrameEventT::mRoiRect = roiRect;
    }

    [[nodiscard]] const std::shared_ptr<const ImageT> &getResultImage() const {
        return mResultImage;
    }

    void setResultImage(const std::shared_ptr<const ImageT> &resultImage) {
        NewFrameEventT::mResultImage = resultImage;
    }

    [[nodiscard]] bool isLastExposure() const {
        return mLastExposure;
    }

    void setLastExposure(bool lastExposure) {
        NewFrameEventT::mLastExposure = lastExposure;
    }

private:
    RectT<unsigned int> mRoiRect;
    std::shared_ptr<const ImageT> mResultImage;
    bool mLastExposure;
};

#endif //FOFI_NEW_FRAME_EVENT_H
