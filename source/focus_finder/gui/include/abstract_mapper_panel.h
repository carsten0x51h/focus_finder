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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_

#include <memory>

#include <QWidget>

#include "../../common/include/logging.h"
#include "../../common/include/mapper_function.h"

class AbstractMapperPanelT : public QWidget {
Q_OBJECT

public:
    explicit AbstractMapperPanelT(QWidget *parent) : QWidget(parent) {}

    ~AbstractMapperPanelT() override {};

    [[nodiscard]] virtual std::shared_ptr<MapperFunctionT> getMapperFunction() const = 0;

    virtual void notifyNewImage() = 0;

signals:

    virtual void valueChangedSignal();
};

#endif /* SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_ */
