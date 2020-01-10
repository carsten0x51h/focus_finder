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

// TODO
/**
 * Convert number of pixels to arcsec.
 *
 * See http://www.paulmcgale.co.uk/tech.htm
 * See http://www.wilmslowastro.com/software/formulae.htm
 *
 * TODO: Put into class PixelArcsecConverterT
 */
//static double pxToArcsec(double px, double inFocalLength /*mm*/,
//		SizeT<float> inPixelSize /*um*/, BinningT inBinning) {
//	const double pixelSizeEq = std::sqrt(
//			inPixelSize.width() * inPixelSize.height());
//	const double arcsecPerPixel = (206.2648 * (double) inBinning.get<0>()
//			* (double) inBinning.get<1>() * pixelSizeEq) / inFocalLength;
//	return arcsecPerPixel * px;
//}

// TODO: Need inverse conversion for symmetry...
