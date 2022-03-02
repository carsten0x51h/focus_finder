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

#ifndef FOFI_STAR_DETAILS_H
#define FOFI_STAR_DETAILS_H FOFI_STAR_DETAILS_H

class StarDetailsT {
public:
    StarDetailsT();

    [[nodiscard]] float getMin() const;

    void setMin(float mMin);

    [[nodiscard]] float getMax() const;

    void setMax(float mMax);

    [[nodiscard]] float getHfd() const;

    void setHfd(float mHfd);

    [[nodiscard]] float getFwhmHorz() const;

    void setFwhmHorz(float mFwhmHorz);

    [[nodiscard]] float getFwhmVert() const;

    void setFwhmVert(float mFwhmVert);

    [[nodiscard]] float getFwhmAvg() const;

    void setFwhmAvg(float mFwhmAvg);

private:
    float mMin{};
    float mMax{};
    float mHfd{}; // TODO: Should be std::shared_ptr<HfdT> ?
    float mFwhmHorz{}; // TODO: Should ne std::shared_ptr<FwhmT> ?
    float mFwhmVert{}; // TODO: Should ne std::shared_ptr<FwhmT> ?
    float mFwhmAvg{}; // TODO: Should be removed
};

#endif /* FOFI_STAR_DETAILS_H */


