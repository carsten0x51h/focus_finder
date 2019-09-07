/**


 * Star recognizer using the CImg library and CCFits.
 *
 * Copyright (C) 2015 Carsten Schmitt
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*Dan Gray's modification notes:

First, I can't believe I found Carsten's website, with this code, 
and all the explanations.
      
      Thanks a lot Carsten!

Secondly, I've learned to apologize for my code.
I learned "C" when I was in my 20's (I'm 62 now). C++ didn't exist.
I went straight from C to C#, 
Now I'm back trying C++, you'll see I have a lot of old habits.
My philosophy is "Make it work like you want it, however you can"

So I believe this will work on most images, quickly and accurately.
But you'll be able to tell my code from Carsten's easily, he is
a much more accomplished C++ programmer than I am!
If it's beautiful code, Carsten probably wrote it.
If it's ugly code, probably me.  :-)

Here's the compile command I use:
g++ star_recognizer.cpp -o star_recognizer -lX11 -lpthread -lCCfits
 -lgsl -lgslcblas -Wl,-rpath=/usr/local -lcfitsio -std=c++0x

For some reason, I had to add -lcfitsio to the command Carsten had 
recommended.

I also had to make the cfitsio with the command:
"make shared"
instead of just:
"make"

This made the extra libraries that CCfits needed.  I also had to 
move the new library files around a bit.....

I made a few improvements, and added several features, as follows.
********************************
 
1. I found that sometimes it wouldn't detect any stars, then sometimes 
thousands, depending on the image, exposure time, etc.  
There's two ints that describe the minStars, and maxStars.
I have them set for:
const int maxStars = 300;
const int minStars = 90;

There's a loop where it finds the stars, and if there's too many stars,
it raises the threshold, and tries again. If there's too few stars, 
it lowers the threshold and tries again.
This can take some time, but I'm assuming most telescope/camera options 
will be fairly consistent, so the -t command switch allows you set the
starting threshold.  Also, where you are in the sky will make a 
difference too.  I recommend taking the first image with a guess from
experience, then using the new threshold as the you analyze the next
image.  The used threshold is written to the "focus.csv" file 
(see below)
 
********************************

2. I noticed that if the outerHfdDiameter was a lot larger than the 
star, the HFD would be way too high.  So I made two routines that
find the star flux diameter.
One starts from the outerHfdDiameter, and works it's way down until
if finds a rise in the flux.  It uses the full circle.
This worked well, but if you started with a large outerHfdDiameter,
it would slow it down a lot.
So now, if you have a look at the function findOuterFluxDiameterBy8,
you see it starts near the centroid, and works it's way out, looking
for a flux value just a bit more than background.
 
But it only samples 8 points at each radius, averages them.
It seems to work just as well as the routine called:
findOuterFluxDiameter, but is a LOT faster.

********************************

3. I added the median for the hfd, this seemed to help the accuracy.

********************************

4. I found I could have more accurate results by rejecting stars
with certain characteristics, if you don't use the -q (more quiet) 
option, you can see the rejected stars, and the reason for rejection.

********************************

5. You can chop the image with two command line switches.
This allows you to reject the outside of your image, it may have coma,
etc.  It also speeds up the process.

-cx 10 will chop out 5 percent of the image from the left, 
and 5% from the right.

-cy 10 will chop out 5 percent of the image from the top, 
and 5% from the bottom. 
 

********************************

6. If you have a sequence of images that have FOCUSnnnnnnnn.* for the
filename, (the nnnn is a sequence of numbers that represent (an int)
focus position), the software will write the information to:
"focus.csv"
 
It appends to this file, so if you run this from a script or something,
delete the "focus.csv" file first, then run a sequence of images, moving
the focuser a certain amount between images, starting with several
on one side of focus, working through focus, and ending after several
images on the other side of focus. Now you can position the focuser 
using the information from the csv file.
The idea, you move the focuser, take an image with a filename:
"FOCUS0012345.fit"
run:
./star_recognizer FOCUS0012345.fit -q
it will create or append to (if existing) the following data:
 
FocusPosition;hfdMedian;hfdAvg;fwhmHorizAvg;fwhmVertAvg;Threshold;
NumberOfStarsUsed;imageFilename

The "focus.csv" file is saved in the same directory as the image 
analyzed.

Do this on inside of focus, and outside of focus, at least 2 each side,
but should be at least 4 each side.

********************************
7. You have the option of displaying images at several key points.
if you use the option -d, it will display the starting image, 
then various other places, like the thresholding, clustering, etc.
Curser statistics are displayed at the top of the window.
Also, the image is stretched before being displayed (but the 
cursor statistics are based on the non-stretched image).   

********************************

a few more things like outputing the elapsed time, etc.
 
********************************
If you want an output image jpeg file, use the -w switch.
The rgb image files are saved with the same location and name 
(with ".jpeg" appended) as the original fits file.

Here's the command line switches:
-d DisplayImages  
-r display the RGB Out image
-q more quiet 
-t 100 starting threshold 
-m only calculate hfd, no fwhm (just a wee bit faster)
-w Write RGB Out image
-cx 10.0 Crop Image percent X (it will keep image centered)
-cx 10.0 Crop Image percent Y (it will keep image  centered)
-o 100 OuterHalfFluxStartingDiameter (pixels)

*/


/**
Here's a bash script that routes output to the file "focus.txt" 
and also to the stdout
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS047170000.ft -q -o 100 -t 96 | tee focus.txt
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS049170000.ft -q -o 100 -t 96 | tee -a focus.txt
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS051170000.ft -q -o 100 -t 144 | tee -a focus.txt
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS053170000.ft -q -o 100 -t 96 | tee -a focus.txt
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS055170000.ft -q -o 100 -t 97 | tee -a focus.txt
./star_recognizer /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS057170000.ft -q -o 100 -t 73 | tee -a focus.txt

Here's the output:
dan@DansLinux:~/bin$ ./analyzeFocusImages.sh
Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS047170000.ft
NumbOfStarsRecognized=140  Threshhold= 96.0 ThresholdLoops=0
hfdMed=11.1   hfdAvg=11.2  fwhmHAvg=5.9   fwhmVAvg=5.8   PixCntAvg=221.2 Used 119 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS047170000].  It took 2.853 seconds

Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS049170000.ft
NumbOfStarsRecognized=248  Threshhold= 96.0 ThresholdLoops=0
hfdMed=5.6    hfdAvg=6.5   fwhmHAvg=3.0   fwhmVAvg=2.7   PixCntAvg=89.3  Used 217 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS049170000].  It took 3.019 seconds

Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS051170000.ft
NumbOfStarsRecognized=251  Threshhold=144.0 ThresholdLoops=0
hfdMed=4.2    hfdAvg=5.3   fwhmHAvg=2.1   fwhmVAvg=1.9   PixCntAvg=53.5  Used 224 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS051170000].  It took 2.911 seconds

Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS053170000.ft
NumbOfStarsRecognized=181  Threshhold= 96.0 ThresholdLoops=0
hfdMed=8.1    hfdAvg=8.9   fwhmHAvg=4.2   fwhmVAvg=4.3   PixCntAvg=157.8 Used 157 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS053170000].  It took 2.825 seconds

Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS055170000.ft
NumbOfStarsRecognized=103  Threshhold= 97.0 ThresholdLoops=0
hfdMed=14.5   hfdAvg=14.6  fwhmHAvg=7.4   fwhmVAvg=7.6   PixCntAvg=388.9 Used 84 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS055170000].  It took 3.198 seconds

Opening File /usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS057170000.ft
NumbOfStarsRecognized=93  Threshhold= 73.0 ThresholdLoops=0
hfdMed=18.9   hfdAvg=18.7  fwhmHAvg=10.1  fwhmVAvg=9.9   PixCntAvg=550.3 Used 63 Stars
Finished....[/usr/share/SiTech/ServoSCommunicator/FocusImages/FOCUS057170000].  It took 3.537 seconds


Here's the focus.csv file contents:
FocusPosition;hfdMedian;hfdAvg;fwhmHorizAvg;fwhmVertAvg;Threshold;NumberOfStarsUsed;imageFilename
47170000;11.057980;11.249972;5.904438;5.837023;96.000000;119;FOCUS047170000.ft
49170000;5.607286;6.479774;2.997662;2.730678;96.000000;217;FOCUS049170000.ft
51170000;4.179106;5.269691;2.061155;1.933197;144.000000;224;FOCUS051170000.ft
53170000;8.148938;8.933505;4.176894;4.262950;96.000000;157;FOCUS053170000.ft
55170000;14.521866;14.583149;7.419919;7.645600;97.000000;84;FOCUS055170000.ft
57170000;18.923294;18.725166;10.124066;9.903115;73.000000;63;FOCUS057170000.ft

Cheers, Dan Gray
   
 */

 
 /*
  * Need to do.
  * 
  * DONE! take threshold as a parameter to make faster.
  * DONE! Make parameter Max flux Diameter amount
  * DONE! Make more quiet mode
  * DONE! Make DisplayImages mode
  * DONE! ignore edge stars
  * DONE! use total flux to ignore dimmer stars
  * DONE! Make parameter Crop amount.
  * DONE! remove ignored stars from list
  * DONE! write output to csv file
  * DONE! add median of HFD
  * DONE! add elapsed time
  * DONE! Make the auto radius be 8 45 degree angles instead of full circles (faster)
  * 	DONE! Get the focus position from filename
  * 	DONE! return the threshold,
  * 
  * subtract dark and/or bias option (later)
  * */
 
#include <iostream>
#include <assert.h>
#include <CImg.h>
#include <CCfits/CCfits>
#include <tuple>
#include <functional>
#include <list>
#include <set>
#include <array>
#include <vector>
#include <sys/time.h>
#include <libgen.h>
#include <string.h>
 
#include <gsl/gsl_multifit_nlin.h>


using namespace std;
using namespace cimg_library;
using namespace CCfits;

int killHotPixels(CImg<float> * ximage, float thresh);
float GetRegionAverage(CImg<float> image, int xloc, int yloc, int len);
bool FileExists (const std::string& name);

typedef tuple<int /*x*/,int /*y*/> PixelPosT;
typedef set<PixelPosT> PixelPosSetT;
typedef list<PixelPosT> PixelPosListT;

typedef tuple<float, float> PixSubPosT;
typedef tuple<float /*x1*/, float /*y1*/, float /*x2*/, float /*y2*/> FrameT;

struct StarInfoT {
  FrameT clusterFrame;
  FrameT cogFrame;
  FrameT hfdFrame;
  PixSubPosT cogCentroid;
  PixSubPosT subPixelInterpCentroid;
  float hfd;
  float fwhmHorz;
  float fwhmVert;
  float maxPixValue;
  float fluxOutsideDiameter;
  float totalFlux;
  int NumOfWhitePixels;
  bool dontUse;
  bool saturated;
};
typedef list<StarInfoT> StarInfoListT;

/**
* Get all pixels inside a radius: http://stackoverflow.com/questions/14487322/get-all-pixel-array-inside-circle
* Algorithm: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
*/
bool
insideCircle(float inX /*pos of x*/, float inY /*pos of y*/, float inCenterX, float inCenterY, float inRadius)
{
  return (pow(inX - inCenterX, 2.0) + pow(inY - inCenterY, 2.0) <= pow(inRadius, 2.0));
}
int SecondsOfDay()
{
     struct timeval tv;
    gettimeofday(&tv, NULL);
//    struct tm info = *gmtime(&tv.tv_sec);
    struct tm info = *localtime(&tv.tv_sec);
    return info.tm_hour * 3600 + info.tm_min * 60 + info.tm_sec;
}

bool FileExists (const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


void
readFile(CImg<float> & inImg, const string & inFilename, long * outBitPix = 0)
{
//  std::auto_ptr<FITS> pInfile(new FITS("FOCUS011811.fit", Read, true));
  std::auto_ptr<FITS> pInfile(new FITS(inFilename, Read, true));

  PHDU & image = pInfile->pHDU(); 

  if (outBitPix) {
    *outBitPix = image.bitpix();
  }

  inImg.resize(image.axis(0) /*x*/, image.axis(1) /*y*/, 1/*z*/, 1 /*1 color*/);
  
  // NOTE: At this point we assume that there is only 1 layer.
  std::valarray<unsigned long> imgData;
  image.read(imgData);
  cimg_forXY(inImg, x, y) { inImg(x, inImg.height() - y - 1) = imgData[inImg.offset(x, y)]; }  
} 
void GetRegionStatistics(CImg<float> image, int xloc, int yloc, int len, float * min, float * max, float * avg)
{
  // Get frame sub img
  int rad = len / 2;
  CImg<float> subImg = image.crop(xloc - rad, yloc - rad, xloc + rad, yloc + rad);
  *avg = subImg.mean();
  *min = subImg.min();
  *max = subImg.max();
}
float GetRegionAverage(CImg<float> image, int xloc, int yloc, int len, float * min, float * max)
{
    // Get frame sub img
  int rad = len / 2;
  CImg<float> subImg = image.crop(xloc - rad, yloc - rad, xloc + rad, yloc + rad);
  *min = subImg.min();
  *max = subImg.max();
  return subImg.mean();
}
float Get5Samples(const CImg<float> & inImg, float *min, float *max)
{
	float avg;
	float avgUpLft, avgUpRt, avgLowLft, avgLowRt, avgCtr;
	int width = inImg.width();
	int height = inImg.height();
	int len = 30;
	
	if(width < len * 4 || height < len * 4) 
	{
    	avg = inImg.mean();
    	*min = inImg.min();
    	*max = inImg.max();
		return avg;
	}
	float min1, min2, min3, min4, min5;
	float max1, max2, max3, max4, max5;
	avgUpLft = GetRegionAverage(inImg, width / 3, height / 3, len, &min1, &max1);
	avgUpRt = GetRegionAverage(inImg, (width / 3) * 2, height / 3, len, &min2, &max2);
	avgLowLft = GetRegionAverage(inImg, (width / 3), (height / 3) * 2, len, &min3, &max3);
	avgLowRt = GetRegionAverage(inImg, (width / 3) * 2, (height / 3) * 2, len, &min4, &max4);
	avgCtr = GetRegionAverage(inImg, width / 2, height / 2, len, &min5, &max5);
	avg = (avgUpLft + avgUpRt + avgLowLft + avgLowRt + avgCtr) / 5;
	*min = (min1 + min2 + min3 + min4 + min5) / 5.0;
	*max = (max1 + max2 + max3 + max4 + max5) / 5.0;
	
	return avg;
}

void MyThreshold(const CImg<float> & inImg, float threshold, CImg<float> * outBinImg)
{	
	float min, max;
	float background = Get5Samples(inImg, &min, &max);
/*
	cimg_forXY(aiImg, x, y) 
	{
		if(inImg(x,y) - max < 0) inImg(x,y) = 0; else inImg(x,y) -= max;
	}
	* */
//printf("bg=%f  min=%f max=%f\n",background, min, max);

	*outBinImg = inImg; // Create a copy


	outBinImg->threshold(threshold + max); 
}

void
thresholdOtsu(const CImg<float> & inImg, long inBitPix, CImg<float> * outBinImg)
{

  CImg<> hist = inImg.get_histogram(pow(2.0, inBitPix));
 
  float sum = 0;
  cimg_forX(hist, pos) { sum += pos * hist[pos]; }
 
  float numPixels = inImg.width() * inImg.height();
  float sumB = 0, wB = 0, max = 0.0;
  float threshold1 = 0.0, threshold2 = 0.0;
  
  cimg_forX(hist, i) {
    wB += hist[i];
 
    if (! wB) { continue; }    
    float wF = numPixels - wB;
    if (! wF) { break; }
    
    sumB += i * hist[i];
    float mF = (sum - sumB) / wF;
    float mB = sumB / wB;
    float diff = mB - mF;
    float bw = wB * wF * pow(diff, 2.0);
    
    if (bw >= max) 
    {
      threshold1 = i;
      if (bw > max) 
      {
         threshold2 = i;
      }
      max = bw;            
    }
  } // end loop
  
  float th = (threshold1 + threshold2) / 2.0;

  *outBinImg = inImg; // Create a copy
  outBinImg->threshold(th); 
}

/**
 * Removes all white neighbours arond pixel from whitePixels
 * if they exist and adds them to pixelsToBeProcessed.
 */
void
getAndRemoveNeighbours(PixelPosT inCurPixelPos, PixelPosSetT * inoutWhitePixels, PixelPosListT * inoutPixelsToBeProcessed)
{
  const size_t _numPixels = 8, _x = 0, _y = 1;
  const int offsets[_numPixels][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 },
                                       { -1, 0 },              { 1, 0 },
                                       { -1, 1 }, { 0, 1 }, { 1, 1 } };
  
  for (size_t p = 0; p < _numPixels; ++p) {
    PixelPosT curPixPos(std::get<0>(inCurPixelPos) + offsets[p][_x], std::get<1>(inCurPixelPos) + offsets[p][_y]);
    PixelPosSetT::iterator itPixPos = inoutWhitePixels->find(curPixPos);
 
    if (itPixPos != inoutWhitePixels->end()) {
      const PixelPosT & curPixPos = *itPixPos;
      inoutPixelsToBeProcessed->push_back(curPixPos);
      inoutWhitePixels->erase(itPixPos); // Remove white pixel from "white set" since it has been now processed
    }
  }
  return;
}

template<typename T> void
clusterStars(const CImg<T> & inImg, StarInfoListT * outStarInfos, int maxStars)
{ 
  PixelPosSetT whitePixels;

 int cnt = 0;
  cimg_forXY(inImg, x, y) 
  {
    if ( inImg(x, y) ) 
    {
      whitePixels.insert(whitePixels.end(), PixelPosT(x, y));
    }
  }
  if(outStarInfos)
  {
	  outStarInfos->clear();
  }
  
  // Iterate over white pixels as long as set is not empty
  while (whitePixels.size()) 
  {
    PixelPosListT pixelsToBeProcessed;
 
    PixelPosSetT::iterator itWhitePixPos = whitePixels.begin();
    pixelsToBeProcessed.push_back(*itWhitePixPos);
    whitePixels.erase(itWhitePixPos);

    FrameT frame(inImg.width(), inImg.height(), 0, 0);

    while(! pixelsToBeProcessed.empty()) {
      PixelPosT curPixelPos = pixelsToBeProcessed.front();

      // Determine boundaries (min max in x and y directions)
      if (std::get<0>(curPixelPos) /*x*/ < std::get<0>(frame) /*x1*/) {	std::get<0>(frame) = std::get<0>(curPixelPos); }
      if (std::get<0>(curPixelPos) /*x*/ > std::get<2>(frame) /*x2*/) { std::get<2>(frame) = std::get<0>(curPixelPos); }
      if (std::get<1>(curPixelPos) /*y*/ < std::get<1>(frame) /*y1*/) {	std::get<1>(frame) = std::get<1>(curPixelPos); }
      if (std::get<1>(curPixelPos) /*y*/ > std::get<3>(frame) /*y2*/) { std::get<3>(frame) = std::get<1>(curPixelPos); }

      getAndRemoveNeighbours(curPixelPos, & whitePixels, & pixelsToBeProcessed);
      pixelsToBeProcessed.pop_front();
// printf("Pix2BPr  Cnt=%d          \r",cnt++); fflush(stdout);

    }
 
    // Create new star-info and set cluster-frame.
    // NOTE: we may use new to avoid copy of StarInfoT...
    StarInfoT starInfo;
    starInfo.clusterFrame = frame;
    int nop = 0;
    for(int ol = std::get<0>(frame); ol < std::get<2>(frame); ol++)
    {
		for(int il = std::get<1>(frame); il < std::get<3>(frame); il++)
		{
			if(inImg(ol,il)) nop++;
		}
// printf("NoOfPx  Cnt=%d          \r",cnt++); fflush(stdout);
	}
//    printf("x1=%f y1=%f x2=%f y2=%f nop=%d\n",std::get<0>(frame),std::get<1>(frame),std::get<2>(frame),std::get<3>(frame),nop);
	starInfo.NumOfWhitePixels = nop;
    if(nop > 3) outStarInfos->push_back(starInfo);
    if(outStarInfos->size() > maxStars*2) break;
  }
}
/*
x1=54.000000 y1=233.000000 x2=56.000000 y2=234.000000
x1=139.000000 y1=648.000000 x2=146.000000 y2=654.000000
x1=338.000000 y1=1144.000000 x2=347.000000 y2=1152.000000
x1=1074.000000 y1=880.000000 x2=1080.000000 y2=885.000000
x1=1089.000000 y1=465.000000 x2=1090.000000 y2=466.000000
x1=1111.000000 y1=759.000000 x2=1117.000000 y2=764.000000
x1=1136.000000 y1=861.000000 x2=1149.000000 y2=868.000000
x1=1259.000000 y1=884.000000 x2=1266.000000 y2=890.000000
x1=1284.000000 y1=948.000000 x2=1287.000000 y2=951.000000
x1=1315.000000 y1=871.000000 x2=1315.000000 y2=872.000000
x1=1319.000000 y1=735.000000 x2=1327.000000 y2=741.000000
x1=1320.000000 y1=835.000000 x2=1324.000000 y2=838.000000
x1=1338.000000 y1=870.000000 x2=1358.000000 y2=890.000000
x1=1363.000000 y1=1202.000000 x2=1370.000000 y2=1207.000000
x1=1400.000000 y1=137.000000 x2=1414.000000 y2=149.000000
x1=1429.000000 y1=947.000000 x2=1432.000000 y2=950.000000
x1=1776.000000 y1=1138.000000 x2=1782.000000 y2=1144.000000
x1=1867.000000 y1=1067.000000 x2=1876.000000 y2=1074.000000
x1=1892.000000 y1=783.000000 x2=1897.000000 y2=787.000000
*/

float
calcIx2(const CImg<float> & img, int x)
{
  float Ix = 0;
  cimg_forY(img, y) { Ix += pow(img(x, y), 2.0) * (float) x; }
  return Ix;
}
 
float
calcJy2(const CImg<float> & img, int y)
{
  float Iy = 0;
  cimg_forX(img, x) { Iy += pow(img(x, y), 2.0) * (float) y; }
  return Iy;
}
 
// Calculate Intensity Weighted Center (IWC)
void
calcIntensityWeightedCenter(const CImg<float> & inImg, float * outX, float * outY)
{
  assert(outX && outY);
  
  // Determine weighted centroid - See http://cdn.intechopen.com/pdfs-wm/26716.pdf
  float Imean2 = 0, Jmean2 = 0, Ixy2 = 0;
  
  for(size_t i = 0; i < inImg.width(); ++i) {
    Imean2 += calcIx2(inImg, i);
    cimg_forY(inImg, y) { Ixy2 += pow(inImg(i, y), 2.0); }
  }

  for(size_t i = 0; i < inImg.height(); ++i) {
    Jmean2 += calcJy2(inImg, i);
  }
  
  *outX = Imean2 / Ixy2;
  *outY = Jmean2 / Ixy2;
}

void
calcSubPixelCenter(const CImg<float> & inImg, float * outX, float * outY, size_t inNumIter = 10 /*num iterations*/)
{
  // Sub pixel interpolation
  float c, a1, a2, a3, a4, b1, b2, b3, b4;
  float a1n, a2n, a3n, a4n, b1n, b2n, b3n, b4n;
 
  assert(inImg.width() == 3 && inImg.height() == 3);
 
  b1 = inImg(0, 0); a2 = inImg(1, 0); b2 = inImg(2, 0);
  a1 = inImg(0, 1);  c = inImg(1, 1); a3 = inImg(2, 1);
  b4 = inImg(0, 2); a4 = inImg(1, 2); b3 = inImg(2, 2);
 
  for (size_t i = 0; i < inNumIter; ++i) {
    float c2 = 2 * c;
    float sp1 = (a1 + a2 + c2) / 4;
    float sp2 = (a2 + a3 + c2) / 4;
    float sp3 = (a3 + a4 + c2) / 4;
    float sp4 = (a4 + a1 + c2) / 4;
    
    // New maximum is center
    float newC = std::max({ sp1, sp2, sp3, sp4 });
    
    // Calc position of new center
    float ad = pow(2.0, -((float) i + 1));
 
    if (newC == sp1) {
      *outX = *outX - ad; // to the left
      *outY = *outY - ad; // to the top
 
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
      *outX = *outX + ad; // to the right
      *outY = *outY - ad; // to the top
 
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
      *outX = *outX + ad; // to the right
      *outY = *outY + ad; // to the bottom
 
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
      *outX = *outX - ad; // to the left
      *outY = *outY + ad; // to the bottom  
 
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
}

void
calcCentroid(const CImg<float> & inImg, const FrameT & inFrame, PixSubPosT * outPixelPos, PixSubPosT * outSubPixelPos = 0, size_t inNumIterations = 10)
{
  // Get frame sub img
  CImg<float> subImg = inImg.get_crop(std::get<0>(inFrame), std::get<1>(inFrame), std::get<2>(inFrame), std::get<3>(inFrame));

  float & xc = std::get<0>(*outPixelPos);
  float & yc = std::get<1>(*outPixelPos);
  
  // 1. Calculate the IWC
  calcIntensityWeightedCenter(subImg, & xc, & yc);

  if (outSubPixelPos) {
    // 2. Round to nearest integer and then iteratively improve.
    int xi = floor(xc + 0.5);
    int yi = floor(yc + 0.5);
  
    CImg<float> img3x3 = inImg.get_crop(xi - 1 /*x0*/, yi - 1 /*y0*/, xi + 1 /*x1*/, yi + 1 /*y1*/);
    
    // 3. Interpolate using sub-pixel algorithm
    float xsc = xi, ysc = yi;
    calcSubPixelCenter(img3x3, & xsc, & ysc, inNumIterations);
    
    std::get<0>(*outSubPixelPos) = xsc;
    std::get<1>(*outSubPixelPos) = ysc;
  }
}
float AverageInRing(const CImg<float> & inImage, float centerX, float centerY, float outRadius, float inRadius)
{
	float sum = 0;
	float AvgCnts;
	
	cimg_forXY(inImage, x, y) 
	{
		if (insideCircle(x, y, centerX, centerY, outRadius)) 
		{
			if (!insideCircle(x, y, centerX, centerY, inRadius))
			{
				AvgCnts++;
				sum += inImage(x, y);
			}
		}
	}
	if(AvgCnts)
	{
		AvgCnts = sum / AvgCnts;
	}
	return AvgCnts;
}
float usedOuterDiameter;
float findOuterFluxDiameterBy8(const CImg<float> & inImage, float centerX, float centerY, float inOuterDiameter, float fluxDecrease)
{
	//for a faster routine, we start near the center, work our way out at 8 different angles, 45 deg's apart.
	//returns the diameter when we get close to the background level.
	float sum = 0;
	float AvgCnts;
	float radius = inOuterDiameter / 2;
	float it707;
	float backGrndAvg;
	sum += inImage(centerX - radius,centerY - 0);
	sum += inImage(centerX + radius,centerY - 0);
	sum += inImage(centerX + 0,centerY + radius);
	sum += inImage(centerX - 0,centerY - radius);
	
	it707 = floor(radius * 0.707 + 0.5);
	sum += inImage(centerX - it707,centerY - it707);
	sum += inImage(centerX + it707,centerY - it707);
	sum += inImage(centerX + it707,centerY + it707);
	sum += inImage(centerX - it707,centerY + it707);
	backGrndAvg = sum / 8;
	
	for(float i = 1; i < radius; i++) 
	{
	sum = 0;
		sum += inImage(centerX - i,centerY - 0);
		sum += inImage(centerX + i,centerY - 0);
		sum += inImage(centerX + 0,centerY + i);
		sum += inImage(centerX - 0,centerY - i);
		
    	it707 = floor(i * 0.707 + 0.5);
		sum += inImage(centerX - it707,centerY - it707);
		sum += inImage(centerX + it707,centerY - it707);
		sum += inImage(centerX + it707,centerY + it707);
		sum += inImage(centerX - it707,centerY + it707);
		AvgCnts = sum / 8;
		if((AvgCnts - backGrndAvg) <= fluxDecrease) 
		{
			usedOuterDiameter = i * 2;
//printf("avgCnts=%f BackGrndAvg=%f fluxDecrease=%f usedOutDia=%f\n",AvgCnts, backGrndAvg, fluxDecrease, usedOuterDiameter);
			return usedOuterDiameter;
		}
	}
	usedOuterDiameter = inOuterDiameter;
//printf("AvgCnts=%f BackGrndAvg=%f fluxDecrease=%f usedOutDia=%f\n",AvgCnts, backGrndAvg, fluxDecrease, usedOuterDiameter);
	return inOuterDiameter;
}
 
float findOuterFluxDiameter(const CImg<float> & inImage, float centerX, float centerY, float inOuterDiameter, float fluxIncrease)
{
	if(inOuterDiameter < 12) return inOuterDiameter;
	float ringRadius = 4;
	float outerRadius = inOuterDiameter / 2;
	float AvgCnts;
	float newAverage;
	float backGrndAvg = AverageInRing(inImage, centerX, centerY, outerRadius, outerRadius - ringRadius);
	outerRadius = outerRadius - ringRadius;
	ringRadius = 2;
	int cnts = 0;
	do
	{
		newAverage = AverageInRing(inImage, centerX, centerY, outerRadius, outerRadius - ringRadius);
		if(newAverage - backGrndAvg > fluxIncrease)
		{
			break;
		}
		outerRadius -= ringRadius;
		cnts++;
	}
	while(outerRadius - ringRadius > 3);
//printf("NewOuterDiameter=%5.1f cnts=%d\n",outerRadius, cnts);
//	outerRadius++; //back it up 1 pixel.
	usedOuterDiameter = outerRadius * 2.0;
	return usedOuterDiameter;
}
 
/**
* Expects star centered in the middle of the image (in x and y) and mean background subtracted from image.
*
* HDF calculation: http://www005.upp.so-net.ne.jp/k_miyash/occ02/halffluxdiameter/halffluxdiameter_en.html
*                  http://www.cyanogen.com/help/maximdl/Half-Flux.htm
*
* NOTE: Currently the accuracy is limited by the insideCircle function (-> sub-pixel accuracy).
* NOTE: The HFD is estimated in case there is no flux (HFD ~ sqrt(2) * inOuterDiameter / 2).
* NOTE: The outer diameter is usually a value which depends on the properties of the optical
*       system and also on the seeing conditions. The HFD value calculated depends on this
*       outer diameter value.
*/
float
calcHfd(const CImg<float> & inImage, float inOuterDiameter, float fluxIncreaseThresh, float * totFlux)
{
  // Sum up all pixel values in whole circle
  float outerRadius = inOuterDiameter / 2;
  float sum = 0, sumDist = 0;
  int centerX = ceil(inImage.width() / 2.0);
  int centerY = ceil(inImage.height() / 2.0);

  if(fluxIncreaseThresh > 0) 
  {
	  usedOuterDiameter = findOuterFluxDiameterBy8(inImage, centerX, centerY, inOuterDiameter, fluxIncreaseThresh);
	  outerRadius = usedOuterDiameter / 2.0;
  }
  else
  {
	usedOuterDiameter = inOuterDiameter;
  }
   
  cimg_forXY(inImage, x, y) {
    if (insideCircle(x, y, centerX, centerY, outerRadius)) {
      sum += inImage(x, y);
      sumDist += inImage(x, y) * sqrt(pow((float) x - (float) centerX, 2.0f) + pow((float) y - (float) centerY, 2.0f));
    }
  }
  // NOTE: Multiplying with 2 is required since actually just the HFR is calculated above
  *totFlux = sum;
  return (sum ? 2.0 * sumDist / sum : sqrt(2.0) * outerRadius);
}
 
/**********************************************************************
* Helper classes
**********************************************************************/
struct DataPointT {
  float x;
  float y;
  DataPointT(float inX = 0, float inY = 0) : x(inX), y(inY) {}
};
  
typedef vector<DataPointT> DataPointsT;
  
struct GslMultiFitDataT {
  float y;
  float sigma;
  DataPointT pt;
};
  
typedef vector<GslMultiFitDataT> GslMultiFitParmsT;
 
 
/**********************************************************************
* Curve to fit to is supplied by traits.
**********************************************************************/
template <class FitTraitsT>
class CurveFitTmplT {
public:
  typedef typename FitTraitsT::CurveParamsT CurveParamsT;
 
  /**
   * DataAccessor allows specifying how x,y data is accessed.
   * See http://en.wikipedia.org/wiki/Approximation_error for expl. of rel and abs errors.
   */
  template<typename DataAccessorT> static int  
  fitGslLevenbergMarquart(const typename DataAccessorT::TypeT & inData, typename CurveParamsT::TypeT * outResults,
          double inEpsAbs, double inEpsRel, size_t inNumMaxIter = 500) {
    GslMultiFitParmsT gslMultiFitParms(inData.size());
      
    // Fill in the parameters
    for (typename DataAccessorT::TypeT::const_iterator it = inData.begin(); it != inData.end(); ++it) {
      size_t idx = std::distance(inData.begin(), it);
      const DataPointT & dataPoint = DataAccessorT::getDataPoint(idx, it);
      gslMultiFitParms[idx].y     = dataPoint.y;
      gslMultiFitParms[idx].sigma = 0.1f;
      gslMultiFitParms[idx].pt    = dataPoint;
    }
 
    // Fill in function info
    gsl_multifit_function_fdf f;
    f.f      = FitTraitsT::gslFx;
    f.df     = FitTraitsT::gslDfx;
    f.fdf    = FitTraitsT::gslFdfx;
    f.n      = inData.size();
    f.p      = FitTraitsT::CurveParamsT::_Count;
    f.params = & gslMultiFitParms;
 
    gsl_vector * guess = gsl_vector_alloc(FitTraitsT::CurveParamsT::_Count);  // Allocate the guess vector
    
    FitTraitsT::makeGuess(gslMultiFitParms, guess);  // Make initial guesses based on the data
    
    // Create a Levenberg-Marquardt solver with n data points and m parameters
    gsl_multifit_fdfsolver * solver = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder,
                                                                  inData.size(), FitTraitsT::CurveParamsT::_Count);
    gsl_multifit_fdfsolver_set(solver, & f, guess);  // Initialize the solver
    
    int status, i = 0;
    
    // Iterate to to find a result
    do {
      i++;
      status = gsl_multifit_fdfsolver_iterate(solver); // returns 0 in case of success
      if (status) {  break; }
      status = gsl_multifit_test_delta(solver->dx, solver->x, inEpsAbs, inEpsRel);
    } while (status == GSL_CONTINUE && i < inNumMaxIter);
    
    // Store the results to be returned to the user (copy from gsl_vector to result structure)
    for (size_t i = 0; i < FitTraitsT::CurveParamsT::_Count; ++i) {
      typename FitTraitsT::CurveParamsT::TypeE idx = static_cast<typename FitTraitsT::CurveParamsT::TypeE>(i);
      (*outResults)[idx] = gsl_vector_get(solver->x, idx);
    }
 
    // Free GSL memory
    gsl_multifit_fdfsolver_free(solver);
    gsl_vector_free(guess);
 
    return status;
  }
};
 
/**********************************************************************
* Gaussian fit traits
**********************************************************************/
class GaussianFitTraitsT {
private:
  
public:
  struct CurveParamsT {
    // b = base, p = peak, c = center in x, w = mean width (FWHM)
    enum TypeE { B_IDX = 0, P_IDX, C_IDX, W_IDX, _Count };
    struct TypeT : public std::array<float, TypeE::_Count> {
      TypeT(const gsl_vector * inVec = 0) {
        for (size_t i = 0; i < TypeE::_Count; ++i) {
          TypeE idx = static_cast<TypeE>(i);
          (*this)[i] = (inVec ? gsl_vector_get(inVec, idx) : 0);
        }
      }
    };
  };
 
  /* Makes a guess for b, p, c and w based on the supplied data */
  static void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) {
    size_t numDataPoints = inData.size();
    float y_mean = 0;
    float y_max = inData.at(0).pt.y;
    float c = inData.at(0).pt.x;
    
    for(size_t i = 0; i < numDataPoints; ++i) {
      const DataPointT & dataPoint = inData.at(i).pt;
 
      y_mean += dataPoint.y;
      
      if(y_max < dataPoint.y) {
        y_max = dataPoint.y;
        c = dataPoint.x;
      }
    }
 
    y_mean /= (float) numDataPoints;
    float w = (inData.at(numDataPoints - 1).pt.x - inData.at(0).pt.x) / 10.0;
    
    gsl_vector_set(guess, CurveParamsT::B_IDX, y_mean);
    gsl_vector_set(guess, CurveParamsT::P_IDX, y_max);
    gsl_vector_set(guess, CurveParamsT::C_IDX, c);
    gsl_vector_set(guess, CurveParamsT::W_IDX, w);
  }
 
  /* y = b + p * exp(-0.5f * ((t - c) / w) * ((t - c) / w)) */
  static float fx(float x, const CurveParamsT::TypeT & inParms) {
    float b = inParms[CurveParamsT::B_IDX];
    float p = inParms[CurveParamsT::P_IDX];
    float c = inParms[CurveParamsT::C_IDX];
    float w = inParms[CurveParamsT::W_IDX];
    float t = ((x - c) / w);
    t *= t;
    return (b + p * exp(-0.5f * t));
  }
 
  /* Calculates f(x) = b + p * e^[0.5*((x-c)/w)] for each data point. */
  static int gslFx(const gsl_vector * x, void * inGslParams, gsl_vector * outResultVec) {    
    CurveParamsT::TypeT curveParams(x);     // Store the current coefficient values
    const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) inGslParams); // Store parameter values
 
    //Execute Levenberg-Marquart on f(x)
    for(size_t i = 0; i < gslParams->size(); ++i) {
      const GslMultiFitDataT & gslData = gslParams->at(i);
      float yi = GaussianFitTraitsT::fx((float) gslData.pt.x, curveParams);
      gsl_vector_set(outResultVec, i, (yi - gslData.y) / gslData.sigma);
    }
    return GSL_SUCCESS;
  }
 
  /* Calculates the Jacobian (derivative) matrix of f(x) = b + p * e^[0.5*((x-c)/w)^2] for each data point */
  static int gslDfx(const gsl_vector * x, void * params, gsl_matrix * J) {
    
    // Store parameter values
    const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) params);
    
    // Store current coefficients
    float p = gsl_vector_get(x, CurveParamsT::P_IDX);
    float c = gsl_vector_get(x, CurveParamsT::C_IDX);
    float w = gsl_vector_get(x, CurveParamsT::W_IDX);
    
    // Store non-changing calculations
    float w2 = w * w;
    float w3 = w2 * w;
    
    for(size_t i = 0; i < gslParams->size(); ++i) {
      const GslMultiFitDataT & gslData = gslParams->at(i);
      float x_minus_c = (gslData.pt.x - c);
      float e = exp(-0.5f * (x_minus_c / w) * (x_minus_c / w));
      
      gsl_matrix_set(J, i, CurveParamsT::B_IDX, 1 / gslData.sigma);
      gsl_matrix_set(J, i, CurveParamsT::P_IDX, e / gslData.sigma);
      gsl_matrix_set(J, i, CurveParamsT::C_IDX, (p * e * x_minus_c) / (gslData.sigma * w2));
      gsl_matrix_set(J, i, CurveParamsT::W_IDX, (p * e * x_minus_c * x_minus_c) / (gslData.sigma * w3));
    }    
    return GSL_SUCCESS;
  }
  
  /* Invokes f(x) and f'(x) */
  static int gslFdfx(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix * J) {
    gslFx(x, params, f);
    gslDfx(x, params, J);
    
    return GSL_SUCCESS;
  }
};
 
typedef list<PixSubPosT> MyDataContainerT;
 
class MyDataAccessorT {
public:
  typedef MyDataContainerT TypeT;
  static DataPointT getDataPoint(size_t inIdx, TypeT::const_iterator inIt) {
    const PixSubPosT & pos = *inIt;
    DataPointT dp(get<0>(pos) /*inIdx*/, get<1>(pos) /*y*/);
    return dp;
  }
};


FrameT
rectify(const FrameT & inFrame)
{
  float border = 3;
  float border2 = 2.0 * border;
  float width = fabs(std::get<0>(inFrame) - std::get<2>(inFrame)) + border2;
  float height = fabs(std::get<1>(inFrame) - std::get<3>(inFrame)) + border2;
  float L = max(width, height);
  float x0 = std::get<0>(inFrame) - (fabs(width - L) / 2.0) - border;
  float y0 = std::get<1>(inFrame) - (fabs(height - L) / 2.0) - border;
  return FrameT(x0, y0, x0 + L, y0 + L);
}

int killHotPixels(CImg<float> * ximage, float thresh)
{
	return 0;
  float min = 0;
  float max = 0;
  int cnt = 0;
  CImg<float> myim = *ximage;

  if(myim.height() < 61 || myim.width() < 61) return 0;
  float avg = GetRegionAverage(myim, 30, 30, 10);
  int width = myim.width();
  int height = myim.height();
  cimg_forXY(myim, x, y) 
  {
    if(y == 0)
    {
	  if(x < width-1) myim(x,y) = avg;
    }
    else if(y == height - 1)
    {
	  if(x < width-1) myim(x,y) = avg;
    }
    else if(x == 0)
    {
	  if(y < height-1) myim(x,y) = avg;
	}
    else if(x == width - 1)
    {
	  if(y < height-1) myim(x,y) = avg;
	}
    else
    {
		if(x < width - 1 && y < height - 1)
		{
			avg = myim(x-1,y-1) + myim(x,y-1) + myim(x+1,y-1) + myim(x+1,y) + myim(x+1,y+1) + myim(x,y+1) + myim(x-1,y+1) + myim(x-1,y);
			avg = avg /= 8;
			if((myim(x,y) - avg) >= thresh) 
			{
				myim(x,y) = ceil(avg);
				cnt++;
			}
		}
    }
  }
  *ximage = myim;
  printf("We killed %d pixels out of %d pixels\n",cnt, width * height);
  return cnt;
}


void DisplayAnImage(CImg<float> imageName, char * WinName, float WinWidth = 1024, bool displayImages = false)
{
  if(!displayImages) return;
  float width = imageName.width();
  float height = imageName.height();

  float Ratio = (float) WinWidth / (float) width;
  char Title[199];
  CImg<float> scrapImg(imageName, false); 
  scrapImg = scrapImg.get_equalize(65535);  
  CImgDisplay imgDisp((unsigned int) (width * Ratio), (unsigned int)(height * Ratio),  "Click a point", 1);
  imgDisp.display(scrapImg);
  imgDisp.set_title(WinName);
  float mX;
  float mY;
  float val = 44;
  float min;
  float max;
  float avg;
  while (! imgDisp.is_closed()) 
   { 
     imgDisp.wait();
     if(imgDisp.is_key() || imgDisp.is_keyENTER() ) imgDisp.close();
     mX = imgDisp.mouse_x() / Ratio;
     mY = imgDisp.mouse_y() / Ratio;
     if(mX < 0.0) mX = 0; if (mX >= width - 1) mX = width - 1;
     if(mY < 0.0) mY = 0; if (mY >= width - 1) mY = width - 1;
     val = imageName((int) mX, (int)mY);
     GetRegionStatistics(imageName, mX, mY, 30, &min, &max,&avg);
     sprintf(Title," %s mX=%-7.1f mY=%-7.1f V=%-7.1f Mn=%-7.1f Mx=%-8.1f avg=%-8.1f",WinName,mX,mY,val,min,max,avg);
     imgDisp.set_title(Title);
   }
}


int main(int argc, char *argv[])
{
  /* outerHfdDiameter depends on pixel size and focal length (and seeing...).
     Later we may calculate it automatically wihth goven focal length and pixel
     size of the camera. For now it is a "best guess" value.
     Now it's a command line value
  */
  struct timeval startTime;
  struct timeval finishTime;
  float outerHfdDiameter = 50;
  StarInfoListT starInfos;
  vector < list<StarInfoT *> > starBuckets;
  CImg<float> img;
  CImg<float> img2;
  long bitPix = 0;
  bool displayImages = false;
  bool displayRGB = false;
  bool displayDebug = true;
  bool writeRGB = false;
  bool medianOnly = false;
  char * imageFilename;
  float startThreshold = 300;
  float cropX = 0;
  float cropY = 0;
  
  if(argc < 2) 
  {
	  printf("Need an image filename!\n");
	  printf("Options: \n-d DisplayImages  \n-q more quiet \n-t 100 starting threshold "
	  "\n-m only calculate hfd, no fwhm (faster)"
	  "\n-r display the RGB Out image"
	  "\n-w Write RGB Out image"
	  "\n-cx 10 Crop Image percent X (it will keep image centered)\n-cx 10.0 Crop Image percent Y (it will keep image  centered)"
	  "\n-o 50 OuterHalfFluxStartingDiameter (pixels)\n");
	  
	  exit(-1);
  }
   imageFilename = argv[1];
   for (int i = 2; i < argc; i++) 
   { 
		if (!strcmp(argv[i], "-d")  )
		{
			displayImages = true;
		} 
		else if (!strcmp(argv[i], "-q") ) 
		{
			displayDebug = false;
		} 
		else if (!strcmp(argv[i], "-m") ) 
		{
			medianOnly = true;
		} 
		else if (!strcmp(argv[i], "-r") ) 
		{
			displayRGB = true;
		} 
		else if (!strcmp(argv[i], "-w") ) 
		{
			writeRGB = true;
		} 
		else if (!strcmp(argv[i], "-t") ) 
		{
			float f = atof(argv[i + 1]); if(f > 9 && f < 30000) startThreshold = f;
		} 
		else if (!strcmp(argv[i], "-cx") ) 
		{
			float f = atof(argv[i + 1]); if(f >=0 && f < 90) cropX = f;
		} 
		else if (!strcmp(argv[i], "-cy") ) 
		{
			float f = atof(argv[i + 1]); if(f >=0 && f < 90) cropY = f;
		} 
		else if (!strcmp(argv[i], "-o") ) 
		{
			float f = atof(argv[i + 1]); if(f >=12 && f <= 350) outerHfdDiameter = f;				
		}
   }

  if(displayDebug)
  {
	  printf("file=%s dispImgs=%d dispDbg=%d strtThresh=%f cropX=%f cropY=%f ohfd=%f\n",
	  imageFilename, displayImages, displayDebug, startThreshold, cropX, cropY, outerHfdDiameter);
  }
  gettimeofday(&startTime, NULL);

  // Read file to CImg
  if(displayDebug) printf("Reading File... %s  \n",imageFilename);
  try {
	  printf("Opening File %s\n",imageFilename);
    readFile(img2, imageFilename, & bitPix);
  } catch (FitsException &) {
    printf("Read FITS failed.");
    exit(1);
    return 1;
  }

  char outCSV_FileNameAndPath[256];
  char imageFileNameNoPath[256];
  char * pathAndFile = strdup(imageFilename);
  
  strcpy(outCSV_FileNameAndPath, dirname(pathAndFile));
  strcat(outCSV_FileNameAndPath,"/focus.csv");
  
  pathAndFile = strdup(imageFilename);
  strcpy(imageFileNameNoPath, basename(pathAndFile));

//  printf("CSVFilename=%s \nJust Image Filename=%s\n",outCSV_FileNameAndPath, imageFileNameNoPath);

  if(bitPix < 0) bitPix = 16;
  float maxPossiblePixValue = pow(2.0, bitPix) - 1;
  if(displayDebug) printf("BitsPix=\%ld\n",bitPix);

	long focusPosition = 0; 
    char * numStrt = strstr(imageFilename,"FOCUS");
    if(numStrt != NULL)
    {
        char * numEnd = strstr(numStrt,".f");
        if(numEnd != NULL)
      {
			numEnd[0] = 0;
			numStrt += 5;
			if(numStrt[0] >= '0' && numStrt[0] <= '9')
			  focusPosition = atol(numStrt);
			else
			  focusPosition = -10000001;
		}
	}
	if(numStrt == NULL)
	{
		focusPosition = -10000002;
		if(displayDebug) printf("focusPosition=%ld\n",focusPosition);
	}
	else
	{
		if(displayDebug) printf("numStrt=%s    focusPosition=%ld\n",numStrt,focusPosition);
	}
  if(cropX > 0 || cropY > 0)
  {
	  float xCtr = floor(img2.width() / 2);
	  float yCtr = floor(img2.height() / 2);
	  cropX = floor(xCtr - cropX * 0.01 * xCtr);
	  cropY = floor(yCtr - cropY * 0.01 * yCtr);
 
	  if(displayDebug) printf("width=%d height=%d xl=%f yt=%f yu=%f yl=%f\n", img2.width(), img2.height(), xCtr-cropX, yCtr-cropY, xCtr + cropX, yCtr + cropY);
      img = img2.crop(xCtr-cropX, yCtr-cropY, xCtr + cropX, yCtr + cropY);
	  if(displayDebug) printf("width=%d height=%d xl=%f yt=%f yu=%f yl=%f\n", img2.width(), img2.height(), xCtr-cropX, yCtr-cropY, xCtr + cropX, yCtr + cropY);
  }
  else
  {
	  img=img2;
      if(displayDebug) printf("width=%d height=%d \n", img2.width(), img2.height());
  }
  if(displayDebug) printf("width=%d height=%d \n", img2.width(), img2.height());
  

  DisplayAnImage(img,(char *) "raw Image",1200, displayImages);
  CImg<float> & aiImg = img.blur_box(3,3,0);
  DisplayAnImage(aiImg,(char *) "after blur",1200, displayImages);

  float min, max;
  float background = Get5Samples(aiImg, &min, &max);
//printf("backG=%f  min=%f\n",background, min);
  background = min;
  //subtract background;

  cimg_forXY(aiImg, x, y) 
  {
	  if(aiImg(x,y) - background < 0) aiImg(x,y) = 0; else aiImg(x,y) -= background;
  }
//  killHotPixels(&img, maxPossiblePixValue * 0.00077);//that would be about 40 pixels at 65535
  DisplayAnImage(aiImg,(char *) "after subBack",1200, displayImages);
   

  if(displayDebug) printf("Thresholding... \n");
  CImg<float> binImg;
//  thresholdOtsu(aiImg, bitPix, & binImg);
  float thresh = startThreshold;
//  /*
const int maxStars = 300;
const int minStars = 90;
  long nos;
  int cntclst = 0;
  do
  {
	  MyThreshold(aiImg,  thresh, & binImg);
	  
	  // Clustering --> In: binary image from thresholding, Out: List of detected stars, subimg-boundaries (x1,y1,x2,y2) for each star
	  if(displayDebug) printf("Clustering...  \n");
	  clusterStars(binImg, & starInfos, maxStars);
	  nos = starInfos.size();
	  if(displayDebug) printf("NumOfStars=%ld Thresh=%f\n",nos,thresh);
	  if(nos < maxStars && nos > minStars) break;
	  if(nos < minStars )
	  {
		thresh *= 0.8;
	  }
	  else
	  {
		thresh *= 1.5;
	  }
	  if(thresh > 32767) break;
	  if(thresh < 10) break;
	  cntclst++;
  }
  while (true);
 
  printf("NumbOfStarsRecognized=%ld  Threshhold=%5.1f ThresholdLoops=%d\n",starInfos.size(), thresh, cntclst);

  DisplayAnImage(binImg,(char *) "Pixelation",1200, displayImages);

  // Calc brightness boundaries for possible focusing stars

  // For each star
  for (StarInfoListT::iterator it = starInfos.begin(); it != starInfos.end(); ++it) 
  {
	  //cog is Center of Gravity
    const FrameT & frame = it->clusterFrame;
    FrameT & cogFrame = it->cogFrame;
    FrameT & hfdFrame = it->hfdFrame;
    PixSubPosT & cogCentroid = it->cogCentroid;
    PixSubPosT & subPixelInterpCentroid = it->subPixelInterpCentroid;
    float & hfd = it->hfd;
    float & fwhmHorz = it->fwhmHorz;
    float & fwhmVert = it->fwhmVert;
    float & maxPixValue = it->maxPixValue;
    bool & saturated = it->saturated;
    
    FrameT squareFrame = rectify(frame);
    
    // Centroid calculation --> In: Handle to full noise reduced image, subimg-boundaries (x1,y1,x2,y2), Out: (x,y) - abs. centroid coordinates
    calcCentroid(aiImg, squareFrame, & cogCentroid, & subPixelInterpCentroid, 10 /* num iterations */);
    std::get<0>(cogCentroid) += std::get<0>(squareFrame);
    std::get<1>(cogCentroid) += std::get<1>(squareFrame);
    std::get<0>(subPixelInterpCentroid) += std::get<0>(squareFrame);
    std::get<1>(subPixelInterpCentroid) += std::get<1>(squareFrame);
    
    
    // Calculate cog boundaries
    float maxClusterEdge = std::max(fabs(std::get<0>(frame) - std::get<2>(frame)), fabs(std::get<1>(frame) - std::get<3>(frame)));
    float cogHalfEdge = ceil(maxClusterEdge / 2.0);
    float cogX = std::get<0>(cogCentroid);
    float cogY = std::get<1>(cogCentroid);
    std::get<0>(cogFrame) = cogX - cogHalfEdge - 1;
    std::get<1>(cogFrame) = cogY - cogHalfEdge - 1;
    std::get<2>(cogFrame) = cogX + cogHalfEdge + 1;
    std::get<3>(cogFrame) = cogY + cogHalfEdge + 1;

    
    // HFD calculation --> In: image, Out: HFD value
    // Subtract mean value from image which is required for HFD calculation
    size_t hfdRectDist = floor(outerHfdDiameter / 2.0);
    std::get<0>(hfdFrame) = cogX - hfdRectDist;
    std::get<1>(hfdFrame) = cogY - hfdRectDist;
    std::get<2>(hfdFrame) = cogX + hfdRectDist;
    std::get<3>(hfdFrame) = cogY + hfdRectDist;

    CImg<float> hfdSubImg = aiImg.get_crop(std::get<0>(hfdFrame), std::get<1>(hfdFrame), std::get<2>(hfdFrame), std::get<3>(hfdFrame));
    maxPixValue = hfdSubImg.max();
    saturated = (maxPixValue >= (maxPossiblePixValue * 0.9));
    
    CImg<float> imgHfdSubMean(hfdSubImg);
    double mean = hfdSubImg.mean();


 // I already subtracted the background from the whole image.....
    cimg_forXY(hfdSubImg, x, y) {
      imgHfdSubMean(x, y) = (hfdSubImg(x, y) < mean ? 0 : hfdSubImg(x, y) - mean);
    }

    // Calc the HFD
    float totflux;
    hfd = calcHfd(imgHfdSubMean, outerHfdDiameter,  /*outer diameter in px*/ maxPossiblePixValue * 0.0005, &totflux);
    it->fluxOutsideDiameter = usedOuterDiameter;
    it->totalFlux = totflux;
    // FWHM calculation --> In: Handle to full noise reduced image, abs. centroid coordinates, Out: FWHM value
    MyDataContainerT vertDataPoints, horzDataPoints;

    cimg_forX(imgHfdSubMean, x) {
      horzDataPoints.push_back(make_pair(x, imgHfdSubMean(x, floor(imgHfdSubMean.height() / 2.0 + 0.5))));
    }
    cimg_forY(imgHfdSubMean, y) {
      vertDataPoints.push_back(make_pair(y, imgHfdSubMean(floor(imgHfdSubMean.width() / 2.0 + 0.5), y)));
    }    
    
    if(medianOnly)
    {
		fwhmHorz = fwhmVert = 0;
	}
    else 
    {
		// Do the LM fit
		typedef CurveFitTmplT<GaussianFitTraitsT> GaussMatcherT;
		typedef GaussMatcherT::CurveParamsT CurveParamsT;
		CurveParamsT::TypeT gaussCurveParmsHorz, gaussCurveParmsVert;
		
		GaussMatcherT::fitGslLevenbergMarquart<MyDataAccessorT>(horzDataPoints, & gaussCurveParmsHorz, 0.1f /*EpsAbs*/, 0.1f /*EpsRel*/);
		fwhmHorz = gaussCurveParmsHorz[CurveParamsT::W_IDX];
		
		GaussMatcherT::fitGslLevenbergMarquart<MyDataAccessorT>(vertDataPoints, & gaussCurveParmsVert, 0.1f /*EpsAbs*/, 0.1f /*EpsRel*/);
		fwhmVert = gaussCurveParmsVert[CurveParamsT::W_IDX];
    }
  }

	float StrCnt = 0;
	float hfdAvg = 0;
	float fwhmVAvg = 0;
	float fwhmHAvg = 0;
	float PixCntAvg = 0;
    int starRemove = 0;
const char *reasons[32] = 
	{ 
		"",
		"Saturated",
		"Too Dim",
		"Saturated, Too Dim",
		"Not Enough Pixels",
		"Saturated, Not Enough Pixels",
		"Too Dim, Not Enough Pixels",
		"Saturated, Too Dim, Not Enough Pixels",
		"CloseToEdge, ",  
		"Saturated, CloseToEdge",
		"CloseToEdge, Too Dim",
		"Saturated, CloseToEdge, Too Dim",
		"CloseToEdge, Not Enough Pixels",
		"Saturated, CloseToEdge, Not Enough Pixels",
		"CloseToEdge, Too Dim, Not Enough Pixels",
		"Saturated, CloseToEdge, Too Dim, Not Enough Pixels",
		"Too Bright",
		"Too Bright, Saturated",
		"Too Bright, Too Dim",
		"Too Bright, Saturated, Too Dim",
		"Too Bright, Not Enough Pixels",
		"Too Bright, Saturated, Not Enough Pixels",
		"Too Bright, Too Dim, Not Enough Pixels",
		"Too Bright, Saturated, Too Dim, Not Enough Pixels",
		"Too Bright, CloseToEdge, ",  
		"Too Bright, Saturated, CloseToEdge",
		"Too Bright, CloseToEdge, Too Dim",
		"Too Bright, Saturated, CloseToEdge, Too Dim",
		"Too Bright, CloseToEdge, Not Enough Pixels",
		"Too Bright, Saturated, CloseToEdge, Not Enough Pixels",
		"Too Bright, CloseToEdge, Too Dim, Not Enough Pixels",
		"Too Bright, Saturated, CloseToEdge, Too Dim, Not Enough Pixels",
		
	};
	float minPixFlux = maxPossiblePixValue / 30;
	float maxPixFlux = maxPossiblePixValue * 8;
//printf("MinFlux=%f  MaxFlux=%f\n",minPixFlux, maxPixFlux);
	for (StarInfoListT::iterator it = starInfos.begin(); it != starInfos.end(); it++) 
	{
		StarInfoT * curStarInfo = & (*it);
		float X = std::get<0>(curStarInfo->cogCentroid);
		float Y = std::get<1>(curStarInfo->cogCentroid);
		int reason = 0;
		if(curStarInfo->saturated) reason = 1;
		if(curStarInfo->totalFlux < minPixFlux) reason |= 2;
		if(curStarInfo->NumOfWhitePixels < 3) reason |= 4;
		if(X < outerHfdDiameter || X > img.width() - outerHfdDiameter || Y < outerHfdDiameter || Y > img.width() - outerHfdDiameter) reason |= 8;
		if(curStarInfo->totalFlux > maxPixFlux ) reason |= 16; 
		if( reason > 0 )
	    {
			if(displayDebug) printf("removing Star X=%f Y=%f  totFlx=%0.1f reason=%s %d\n",X, Y, curStarInfo->totalFlux, reasons[reason], reason);
			curStarInfo->dontUse = true;
			StrCnt ++;
		}
		else
		{
			curStarInfo->dontUse = false;
		}
		
	}
	StrCnt = 0;
	
	std::list<float> hfdList;
	
	for (StarInfoListT::iterator it = starInfos.begin(); it != starInfos.end(); it++) 
	{
		StarInfoT * curStarInfo = & (*it);
		if(curStarInfo->dontUse)
		{
		}
		else
		{
			float X = std::get<0>(curStarInfo->cogCentroid);
			float Y = std::get<1>(curStarInfo->cogCentroid);

			hfdAvg += curStarInfo->hfd;
			hfdList.push_back(curStarInfo->hfd);
			fwhmHAvg += curStarInfo->fwhmHorz;
			fwhmVAvg += curStarInfo->fwhmVert;
			PixCntAvg += curStarInfo->NumOfWhitePixels;
			if(displayDebug)
			{
			printf("cenX=%-7.1f cenY=%-7.1f hfd=%-5.1f totFlx=%-8.1f fwhmH=%-5.1f fwhmV=%-5.1f PixCnt=%-5d fluxOutDia=%-5.1f\n",
				X, Y, curStarInfo->hfd, curStarInfo->totalFlux, curStarInfo->fwhmHorz,curStarInfo->fwhmVert,
				curStarInfo->NumOfWhitePixels, curStarInfo->fluxOutsideDiameter);
			}
			StrCnt++;
		}
	}
	hfdList.sort();
	float hfdMedian;
	auto itr = hfdList.begin();
	    // n is even
    if( hfdList.size() % 2 == 0 ) 
    {
        for( int i = 0 ; i < hfdList.size() / 2 ; i ++ ) 
        {
            itr++;
        }
        hfdMedian = ( (float)*itr + *--itr ) / 2;
    }
    // n is odd
    else 
    {
        for( int i = 0 ; i < hfdList.size() / 2 ; i ++ ) 
        {
            itr++;
        }
        hfdMedian = *itr;
    }
	if(StrCnt)
	{
		hfdAvg /= StrCnt;
		fwhmHAvg /= StrCnt;
		fwhmVAvg /= StrCnt;
		PixCntAvg /= StrCnt;
	}	
	char dataString[266];
    sprintf(dataString,"hfdMed=%-5.1f  hfdAvg=%-5.1f fwhmHAvg=%-5.1f fwhmVAvg=%-5.1f PixCntAvg=%-5.1f Used %d Stars\n",
		hfdMedian, hfdAvg, fwhmHAvg, fwhmVAvg, PixCntAvg, (int) StrCnt);
    printf("%s",dataString);
    
    
    if(focusPosition > -10000000)
    {
		bool writeHeader = false;
		if(!FileExists(outCSV_FileNameAndPath)) writeHeader = true;
		FILE * f;
		
		f = fopen(outCSV_FileNameAndPath,"a");
		if(f > 0)
		{
			if(writeHeader) 
			{
				fprintf(f,"FocusPosition;hfdMedian;hfdAvg;fwhmHorizAvg;fwhmVertAvg;Threshold;NumberOfStarsUsed;imageFilename\n");
			}
			
         fprintf(f,"%ld;%f;%f;%f;%f;%f;%d;%s\n", focusPosition, hfdMedian, hfdAvg, fwhmHAvg, fwhmVAvg, thresh, (int) StrCnt, (char *) imageFileNameNoPath);
            
			fclose(f);
			if(displayDebug) printf("Appended Data to file:[%s]\n",outCSV_FileNameAndPath);
		}
		else
		{ 
			printf("Couldn't create output file:[%s]\n",outCSV_FileNameAndPath);
		}
	}

  gettimeofday(&finishTime, NULL);
  double elapsedSecs = ((double)finishTime.tv_sec-(double)startTime.tv_sec) * 1000000.0;
  double elapsedUsecs = ((double)finishTime.tv_usec-(double)startTime.tv_usec);
  elapsedSecs = (elapsedUsecs + elapsedSecs) / 1000000.0;
  printf("Finished....[%s].  It took %0.3f seconds\n",imageFilename,elapsedSecs);
  
  if(writeRGB || displayRGB)
  {    
  // Create result image
  const int factor = 1;
   float FontSize; 
   if(displayDebug) printf("Making JPeg... \n");
	    // Create RGB image from fits file to paint boundaries and centroids (just for visualization)
	  float min = img.min(), mm = img.max() - min;
	  if(displayDebug) printf("Creating RGB... \n");
	  CImg<unsigned char>	rgbImg(img.width(), img.height(), 1 /*depth*/, 3 /*3 channels - RGB*/);
	  cimg_forXY(img, x, y) 
	  {
		int value = 255.0 * (img(x,y) - min) / mm;
		rgbImg(x, y, 0 /*red*/) = value;
		rgbImg(x, y, 1 /*green*/) = value;
		rgbImg(x, y, 2 /*blue*/) = value;
	  }

      CImg<unsigned char> & rgbResized = rgbImg.resize(factor * rgbImg.width(), factor * rgbImg.height(),
						   -100 /*size_z*/, -100 /*size_c*/, 1 /*interpolation_type*/);  
    FontSize = rgbResized.width() / 50; if (FontSize < 15) FontSize = 15;
  // Draw cluster boundaries and square cluster boundaries
  const unsigned char red[3] = { 255, 0, 0 }, green[3] = { 0, 255, 0 }, yellow[3] = { 255, 255, 0 };
  const unsigned char  black[3] = { 0, 0, 0 }, blue[3] = { 0, 0, 255 }, white[3] = { 255, 255, 255 };
  const size_t cCrossSize = 3;
  const float LineLength = 10;
  // Mark all stars in RGB image

	  for (StarInfoListT::iterator it = starInfos.begin(); it != starInfos.end(); ++it) 
	  {
		StarInfoT * curStarInfo = & (*it);
        if(curStarInfo->dontUse) continue;
		float X = std::get<0>(curStarInfo->cogCentroid);
		float Y = std::get<1>(curStarInfo->cogCentroid);
		
		rgbImg.draw_circle(floor(factor * X + 0.5), floor(factor * Y + 0.5), (factor * it->fluxOutsideDiameter / 2) + 4, red, 1 /*pattern*/, 1 /*opacity*/);
		rgbImg.draw_circle(floor(factor * X + 0.5), floor(factor * Y + 0.5), (factor * it->fluxOutsideDiameter / 2) + 6, red, 1 /*pattern*/, 1 /*opacity*/);
	  
	  }
	  rgbImg.draw_text(6, 24, dataString, white /*fg color*/, black /*bg color*/, 1.0 /*opacity*/, FontSize /*font-size*/);
	  
	  if(focusPosition > -1000000)
		{
			char focPos[88];
			sprintf(focPos,"FocusPosition=%ld Thresh=%0.0f",focusPosition, thresh);
			rgbImg.draw_text(6, FontSize + 10, (char *) focPos, white /*fg color*/, black /*bg color*/, 1.0 /*opacity*/, FontSize /*font-size*/);
		}
		
	  char outImageFilename[256] ;
	  strcpy(outImageFilename,imageFilename);
	  strcat(outImageFilename,".jpeg");
  
    if(writeRGB) rgbResized.save(outImageFilename);
    DisplayAnImage(rgbImg, (char *)"Output",(float) 1600 , displayRGB);
  }
 
  printf("\n");
  return 0;
}

