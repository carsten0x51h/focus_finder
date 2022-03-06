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

#include <optional>

// TODO: Split different calculation methods to different classes / files?
#include "include/centroid.h"
#include "include/centroid_type.h"
#include "include/throw_if.h"
#include "include/image.h"
#include "include/logging.h"
#include "include/point.h"
#include "include/tuple_printer.h"

void CentroidT::calcCentroid2ndMoment(const ImageT &inImg,
                                      PointT<float> *outCentroidPos) {
    THROW_IF(Centroid, outCentroidPos == nullptr, "outCentroidPos expected to be set.")

    LOG(trace)
        << "starCentroid - image dimension (w h)=(" << inImg.width() << " "
        << inImg.height() << ")" << std::endl;

    // Find max pixel value and position
    // TODO: We may use min_max function instead!?
    double maxValue = 0;
    int xPosMax = 0;
    int yPosMax = 0;

    // TODO: Use CImg max() function?
    cimg_forXY(inImg, x, y) {
            if (maxValue < inImg(x, y)) {
                maxValue = inImg(x, y);
                xPosMax = x;
                yPosMax = y;
            }
        }

    LOG(trace)
        << "starCentroid - subImg maxValue: " << maxValue << ", xPosMax: "
        << xPosMax << ", yPosMax: " << yPosMax << std::endl;

    // TODO: The cropping below is probably nor required!!!
    const int SIZE = inImg.width();
    const int BACK = (int) std::ceil((float) inImg.width() / 2.0F) - 1;

    std::valarray<float> vals(
            SIZE * SIZE); // this array will contain all the intensities of the pixels in the SIZE*SIZE grid
    std::valarray<float> dist(SIZE * SIZE); // contains all distances for each pixel to the central pixel
    std::valarray<float> distX(SIZE * SIZE); // contains the X distance for each pixel to the central pixel
    std::valarray<float> distY(SIZE * SIZE); // contains the Y distance for each pixel to the central pixel

    // X and Y are now central brightest pixel in SIZExSIZE grid
    // TODO: Check!! SIZE should be configured?!! SIZE should be odd!?!

    int startX = xPosMax - BACK, startY = yPosMax - BACK;
    const ImageT &subInImg = inImg.get_crop(startX, startY,
                                            startX + SIZE - 1, startY + SIZE - 1);


    LOG(debug) << "CentroidT::calcCentroid2ndMoment - SIZE: " << SIZE << " --> subInImg - w=" << subInImg.width()
               << ", h=" << subInImg.height() << std::endl;


    LOG(trace)
        << "starCentroid - subInImg size (w h)=(" << subInImg.width() << " "
        << subInImg.height() << ")" << std::endl;

    cimg_forXY(subInImg, x, y) {
            const long off = subInImg.offset(x, y);
            vals[off] = subInImg(x, y); // save pixel intensity
            distX[off] = (double) x - (double) xPosMax; // x distance of pixel from top left
            distY[off] = (double) y - (double) yPosMax; // y distance of pixel from top left
            dist[off] = sqrt(
                    pow((double) distX[off], 2.0) +
                    pow((double) distY[off], 2.0)); // distance of pixel from top-left, uses x and y to find hypotenuse
        }

    // 3. Find valueLow and valueHigh in grid
    float levelLow = vals[0], levelHigh = vals[0];

    for (size_t i = 0; i < vals.size(); ++i) {
        if (vals[i] < levelLow) {
            levelLow = vals[i];
        }
        if (vals[i] > levelHigh) {
            levelHigh = vals[i];
        }
    }

    LOG(trace)
        << "starCentroid - subInImg levelLow: " << levelLow << ", levelHigh: "
        << levelHigh << std::endl;

    // 4. Create secondMoment array
    // TODO: Simply code - once it works and does what it should!
    typedef std::vector<double> MomentValuesT;
    MomentValuesT secondMoment; // Do we know size before?!
    double levelCurrHighest = levelHigh;
    double levelNextHighest = 0;
    double currentSum = 0;

    // Find the second moment for levelHigh - for every pixel in the grid, check if intesity = current intensity level, find next highest intensity in grid
    for (size_t i = 0; i < vals.size(); ++i) {
        if (vals[i] == levelCurrHighest) {
            currentSum += sqrt((double) dist[i]); //sum the squares of the distances of the pixel from the center
        }
        if (vals[i] < levelCurrHighest && vals[i] > levelNextHighest) {
            levelNextHighest = vals[i]; // next highest intensity in grid, used for iterations
        }
    }

    levelCurrHighest = levelNextHighest; // Set for next iteration
    secondMoment.push_back(currentSum); // Add currentSum to the secondMoment array

    // Find second moment for all remaining levels
    int count = 1; // Keep track of level

    // FIXME / TODO: Here we had while(levelCurrHighest >= levelLow) {.... which lead to an endless loop (levelCurrHighest == levelLow == 0...)
    while (levelCurrHighest > levelLow) {
        currentSum = 0;
        levelNextHighest = 0;

        // For every pixel in the grid, check if intesity = current intensity level, find next highest intensity in grid
        for (size_t i = 0; i < vals.size(); i++) {
            if (vals[i] == levelCurrHighest) {
                currentSum += sqrt((double) dist[i]); // Sum the squares of the distances of the pixel from the center
            }
            if (vals[i] < levelCurrHighest && vals[i] > levelNextHighest) {
                levelNextHighest = vals[i]; // Next highest intensity in grid, used for iterations
            }
        }
        currentSum += secondMoment.at(count - 1);
        secondMoment.push_back(currentSum);
        count++;

        levelCurrHighest = levelNextHighest;
    } // end while

    LOG(trace)
        << "starCentroid - List of 2nd moments:" << std::endl;
    for (auto it = secondMoment.begin();
         it != secondMoment.end(); ++it) {
        LOG(trace)
            << "Level: "
            << std::distance<MomentValuesT::const_iterator>(
                    secondMoment.begin(), it) << " - value: " << *it
            << std::endl;
    }

    // 5. Create filteredSecondMoment array
    MomentValuesT filteredSecondMoment(secondMoment);

    // For each value in second moment array, compute a normalized moving 5 point center weighted filter
    // (weights of 1,2,3,2,1, Normalization factor = 9) does not compute the first two or last two values of second moment.
    for (size_t i = 2; i < secondMoment.size() - 2; ++i) {
        filteredSecondMoment[i] = ((secondMoment[i - 2]
                                    + secondMoment[i - 1] * 2.0 + secondMoment[i] * 3.0
                                    + secondMoment[i + 1] * 2.0 + secondMoment[i + 2]) / 9.0);
    }

    LOG(trace)
        << "starCentroid - List of filtered 2nd moments:" << std::endl;
    for (auto it = filteredSecondMoment.begin();
         it != filteredSecondMoment.end(); ++it) {
        LOG(trace)
            << "Level: "
            << std::distance<MomentValuesT::const_iterator>(
                    filteredSecondMoment.begin(), it)
            << " - Filtered 2nd-Moment: " << *it << std::endl;
    }

    // 6. Find the Threshold
    int threshInt = 0; // Holds the threshold pixel
    bool check = true;
    int level = (int) filteredSecondMoment.size() - 3; // Set search index to the levelLow and begin search there

    while (check) {
        // If the difference between level and lower level is < 100, threshold has been found
        double checkVal = fabs(
                filteredSecondMoment[level] - filteredSecondMoment[level - 1]);
        if (checkVal < 100) {
            threshInt = level;
            check = false;
        }
        level--;
        if (level <= 1) {
            throw CentroidExceptionT("Threshold could not be found!");
        }
    }
    double threshold = vals[threshInt];

    LOG(trace)
        << "starCentroid - threshInt: " << threshInt << ", threshold: " << threshold
        << std::endl;

    // 7. Sum of all intensities, xSum, ySum, Threshold & Calculate Centroid
    double sum = 0; // Sum of all of the intensities in the grid
    double xSum = 0; // Sum of the product of the intensity of a pixel times it's x-distance from the center of the grid
    double ySum = 0; // Sum of the product of the intensity of a pixel times it's y-distance from the center of the grid

    // Threshold data and calculate the sums
    for (size_t i = 0; i < vals.size(); i++) {
        if (vals[i] <= threshold) {
            vals[i] = 0; // Pix intensity <= thres, set to 0
        } else {
            vals[i] = vals[i] - threshold; // Pix intensity > thres, calc subtraction
        }
        sum += vals[i]; // Sum of intensities
        xSum += vals[i] * (double) distX[i]; // Sum of intensities * distance X
        ySum += vals[i] * (double) distY[i]; // Sum of intensities * distance Y
    }

    // Calculate centroid
    if (sum < std::numeric_limits<double>::epsilon()) {
        throw CentroidExceptionT("Unable to determine centroid.");
    }

    std::get<0>(*outCentroidPos) = xPosMax + xSum / sum;
    std::get<1>(*outCentroidPos) = yPosMax + ySum / sum;
}

void CentroidT::calcCentroidSubPixel(const ImageT &inImg,
                                     PointT<float> inCenter, PointT<float> *outSubPixelCenter,
                                     size_t inNumIterations) {
    THROW_IF(Centroid, outSubPixelCenter == nullptr, "outSubPixelCenter expected to be set.")

    // 2. Round to nearest integer and then iteratively improve.
    int xi = floor(std::get<0>(inCenter) + 0.5);
    int yi = floor(std::get<1>(inCenter) + 0.5);

    ImageT img3x3 = inImg.get_crop(xi - 1 /*x0*/, yi - 1 /*y0*/, xi + 1 /*x1*/,
                                   yi + 1 /*y1*/);

    // 3. Interpolate using sub-pixel algorithm
    auto xsc = (float) xi;
    auto ysc = (float) yi;

    // Sub pixel interpolation
    float c;
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float a1n, a2n, a3n, a4n;
    float b1n, b2n, b3n, b4n;

    THROW_IF(Centroid, img3x3.width() != 3 || img3x3.height() != 3,
             "Expected image for sub-pixel calculation being 3x3.")

    b1 = img3x3(0, 0);
    a2 = img3x3(1, 0);
    b2 = img3x3(2, 0);
    a1 = img3x3(0, 1);
    c = img3x3(1, 1);
    a3 = img3x3(2, 1);
    b4 = img3x3(0, 2);
    a4 = img3x3(1, 2);
    b3 = img3x3(2, 2);

    for (size_t i = 0; i < inNumIterations; ++i) {
        float c2 = 2 * c;
        float sp1 = (a1 + a2 + c2) / 4;
        float sp2 = (a2 + a3 + c2) / 4;
        float sp3 = (a3 + a4 + c2) / 4;
        float sp4 = (a4 + a1 + c2) / 4;

        // New maximum is center
        float newC = std::max({sp1, sp2, sp3, sp4});

        // Calc position of new center
        float ad = pow(2.0, -((float) i + 1));

        if (newC == sp1) {
            xsc = xsc - ad; // to the left
            ysc = ysc - ad; // to the top

            // Calculate new sub pixel values
            b1n = (a1 + a2 + 2 * b1) / 4;
            b2n = (c + b2 + 2 * a2) / 4;
            b3n = sp3;
            b4n = (b4 + c + 2 * a1) / 4;
            a1n = (b1n + c + 2 * a1) / 4;
            a2n = (b1n + c + 2 * a2) / 4;
            a3n = sp2;
            a4n = sp4;

        } else if (newC == sp2) {
            xsc = xsc + ad; // to the right
            ysc = ysc - ad; // to the top

            // Calculate new sub pixel values
            b1n = (2 * a2 + b1 + c) / 4;
            b2n = (2 * b2 + a3 + a2) / 4;
            b3n = (2 * a3 + b3 + c) / 4;
            b4n = sp4;
            a1n = sp1;
            a2n = (b2n + c + 2 * a2) / 4;
            a3n = (b2n + c + 2 * a3) / 4;
            a4n = sp3;
        } else if (newC == sp3) {
            xsc = xsc + ad; // to the right
            ysc = ysc + ad; // to the bottom

            // Calculate new sub pixel values
            b1n = sp1;
            b2n = (b2 + 2 * a3 + c) / 4;
            b3n = (2 * b3 + a3 + a4) / 4;
            b4n = (2 * a4 + b4 + c) / 4;
            a1n = sp4;
            a2n = sp2;
            a3n = (b3n + 2 * a3 + c) / 4;
            a4n = (b3n + 2 * a4 + c) / 4;
        } else {
            xsc = xsc - ad; // to the left
            ysc = ysc + ad; // to the bottom

            // Calculate new sub pixel values
            b1n = (2 * a1 + b1 + c) / 4;
            b2n = sp2;
            b3n = (c + b3 + 2 * a4) / 4;
            b4n = (2 * b4 + a1 + a4) / 4;
            a1n = (b4n + 2 * a1 + c) / 4;
            a2n = sp1;
            a3n = sp3;
            a4n = (b4n + 2 * a4 + c) / 4;
        }

        c = newC; // Oi = Oi+1

        a1 = a1n;
        a2 = a2n;
        a3 = a3n;
        a4 = a4n;

        b1 = b1n;
        b2 = b2n;
        b3 = b3n;
        b4 = b4n;
    }

    std::get<0>(*outSubPixelCenter) = xsc;
    std::get<1>(*outSubPixelCenter) = xsc;
}

double CentroidT::calcIx2(const ImageT &img, int x) {
    double Ix = 0;
    cimg_forY(img, y) {
        Ix += std::pow(img(x, y), 2.0) * (float) x;
    }
    return Ix;
}

double CentroidT::calcJy2(const ImageT &img, int y) {
    double Iy = 0;
    cimg_forX(img, x) {
        Iy += std::pow(img(x, y), 2.0) * (float) y;
    }
    return Iy;
}

// Calculate Intensity Weighted Center (IWC)
void CentroidT::calcIntensityWeightedCenter(const ImageT &inImg,
                                            PointT<float> *outCentroidPos) {
    THROW_IF(Centroid, outCentroidPos == nullptr, "outCentroidPos expected to be set.")

    // Determine weighted centroid - See http://cdn.intechopen.com/pdfs-wm/26716.pdf
    double Imean2 = 0;
    double Jmean2 = 0;
    double Ixy2 = 0;

    for (int i = 0; i < inImg.width(); ++i) {
        Imean2 += calcIx2(inImg, i);
        cimg_forY(inImg, y) {
            Ixy2 += pow(inImg(i, y), 2.0);
        }
    }

    for (int i = 0; i < inImg.height(); ++i) {
        Jmean2 += calcJy2(inImg, i);
    }

    std::get<0>(*outCentroidPos) = Imean2 / Ixy2;
    std::get<1>(*outCentroidPos) = Jmean2 / Ixy2;

    LOG(info) << "CentroidT::calcIntensityWeightedCenter - Calculated centroid relative pos=("
              << std::get<0>(*outCentroidPos) << ", " << std::get<1>(*outCentroidPos) << ")..."
              << std::endl;
}


std::optional<PointT<float> > CentroidT::calculate(const ImageT &inImg,
                                                   CentroidTypeT::TypeE inCalcType, bool inSubMean, ImageT *outImg) {

    PointT<float> relCenter;
    bool centroidFound;

    if (inImg.width() <= 0 || inImg.height() <= 0) {
        return std::nullopt;
    }

    // Noise reduction
    // AD noise reduction --> In: Loaded image, Out: Noise reduced image
    // http://cimg.sourceforge.net/reference/structcimg__library_1_1CImg.html
    // NOTE: Can throw CImgArgumentException
    ImageT aiImg = inImg.get_blur_anisotropic(130.0F, /*amplitude*/
                                              0.7F, /*sharpness*/
                                              0.3F, /*anisotropy*/
                                              0.6F, /*alpha*/
                                              1.1F, /*sigma*/
                                              0.8F, /*dl*/
                                              30, /*da*/
                                              2, /*gauss_prec*/
                                              0, /*interpolation_type*/
                                              false /*fast_approx*/
    );

    // Sub mean image if desired
    if (inSubMean) {
        double mean = aiImg.mean();
        cimg_forXY(aiImg, x, y) {
                aiImg(x, y) = (aiImg(x, y) < mean ? 0 : aiImg(x, y) - mean);
            }
    }

    // TODO: "zoom" (interpolate) image by factor...

    switch (inCalcType) {
        case CentroidTypeT::IWC: {
            calcIntensityWeightedCenter(aiImg, &relCenter); // center is relative

            // This method does not allow an evaluation if a real "star" was detected.
            // Therefore it always returns true.
            centroidFound = true;
            break;
        }
        case CentroidTypeT::IWC_SUB: {
            PointT<float> centerIwc;
            calcIntensityWeightedCenter(aiImg, &centerIwc);
            calcCentroidSubPixel(aiImg, centerIwc, &relCenter,
                                 20 /*numIterations*/);

            // This method does not allow an evaluation if a real "star" was detected.
            // Therefore it always returns true.
            centroidFound = true;
            break;
        }
        case CentroidTypeT::MOMENT2: {
            try {
                calcCentroid2ndMoment(aiImg, &relCenter);

                centroidFound = true;
            } catch (CentroidExceptionT &exc) {
                centroidFound = false;
                LOG(warning)
                    << "Detection of centroid using MOMENT2 failed: " << exc.what()
                    << std::endl;
            }
            break;
        }
        default: {
            THROW_IF(Centroid, true, "Invalid CentroidTypeT.")
        }
    }

    /**
     * We return a copy of the zoomed (interpolated) image.
     */
    if (outImg != nullptr) {
        *outImg = aiImg;
    }

    // Debug output only...
    if (centroidFound) {
        LOG(info)
            << "CentroidT::calculate() - centroid found - x: " << std::get<0>(relCenter) << ",y: "
            << std::get<1>(relCenter)
            << std::endl;
    } else {
        LOG(info)
            << "CentroidT::calculate() - centroid NOT found." << std::endl;
    }

    return (centroidFound ? std::optional<PointT<float> >{relCenter} : std::nullopt);
}


CentroidT::CentroidT() = default;

CentroidT::CentroidT(const ImageT &inImg, CentroidTypeT::TypeE inCalcType,
                     bool inSubMean) {
    this->set(inImg, inCalcType, inSubMean);
}

void CentroidT::set(const ImageT &inImg, CentroidTypeT::TypeE inCalcType,
                    bool inSubMean) {
    mCenterOpt = CentroidT::calculate(inImg, inCalcType,
                                      inSubMean, &mImg);
}

bool CentroidT::valid() const {
    return mCenterOpt.has_value();
}

const ImageT &CentroidT::getResultImage() const {
    return mImg;
}

std::optional<PointT<float> > CentroidT::getCenter() const {
    return mCenterOpt;
}

