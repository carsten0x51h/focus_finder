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

#include <QPainter>

#include "../common/include/fofi_project_info.h"

#include "include/fofi_splash_widget.h"
#include "include/about_dialog.h"

#include "ui_about_dialog.h"

//void AboutDialogT::paintEvent(QPaintEvent * /*event*/) {
//}

AboutDialogT::AboutDialogT(QWidget *parent) : QDialog(parent),
                                              m_ui(new Ui::AboutDialog) {
    // Setup UI
    m_ui->setupUi(this);

    setFixedSize(size());


    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    mFoFiSplashWidget = new FoFiSplashWidgetT(m_ui->widget);
    mFoFiSplashWidget->setSizePolicy(sizePolicy);
    m_ui->layFoFiSplashWidget->addWidget(mFoFiSplashWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    repaint();
}

// TODO: Should mFoFiSplashWidget be deleted?
AboutDialogT::~AboutDialogT() = default;
