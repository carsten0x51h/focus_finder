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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_DEVICE_MANAGER_FACTORY_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_DEVICE_MANAGER_FACTORY_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_DEVICE_MANAGER_FACTORY_H_

#include <memory>

#include "../../common/include/device_manager.h"

class QDialog;

class QWidget;

class DeviceManagerDialogFactoryT {
public:
    static std::shared_ptr<QDialog> getInstance(QWidget *parent, std::shared_ptr<DeviceManagerT> deviceManager);
};


#endif /* SOURCE_FOCUS_FINDER_GUI_INCLUDE_DEVICE_MANAGER_FACTORY_H_ */
