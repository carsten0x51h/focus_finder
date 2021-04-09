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

#include <sstream>
#include <QPainter>

#include "../common/include/fofi_project_info.h"

#include "include/fofi_splash_widget.h"


std::string FoFiSplashWidgetT::createFoFiVersionString() {
    std::stringstream ss;

    ss << "V" << FoFiProjectInfoT::getVersionString()
       << " (" << FoFiProjectInfoT::getVersionRevision() << ")";

    return ss.str();
}

FoFiSplashWidgetT::FoFiSplashWidgetT(QWidget *parent) : QLabel(parent) {

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    setSizePolicy(sizePolicy);

    mSpashScreenPixmap = new QPixmap(":/res/fofi_splash_screen.png");

    repaint();
}

FoFiSplashWidgetT::~FoFiSplashWidgetT() {
    delete mSpashScreenPixmap;
}

void FoFiSplashWidgetT::paintEvent(QPaintEvent *event) {

    QPainter p(this);

    p.drawPixmap(0,0, *mSpashScreenPixmap);

    static const int LEFT_BORDER = 40;
    static const int TOP_BORDER = 310;
    static const int PROJECT_NAME_FONT_SIZE = 62;
    static const int DESCRIPTION_FONT_SIZE = 25;
    static const int VERSION_FONT_SIZE = 18;
    static const int DETAILS_FONT_SIZE = 14;

    static const int PROJECT_NAME_Y_POS = TOP_BORDER;
    static const int DESCRIPTION_Y_POS = TOP_BORDER + PROJECT_NAME_FONT_SIZE;
    static const int VERSION_NUMBER_Y_POS = DESCRIPTION_Y_POS + 2 * DESCRIPTION_FONT_SIZE;
    static const int WEBSITE_Y_POS = VERSION_NUMBER_Y_POS + 3 * VERSION_FONT_SIZE;
    static const int CONTACT_Y_POS = WEBSITE_Y_POS + 2 * DETAILS_FONT_SIZE;

    p.setPen(Qt::white);

    QFont font = p.font() ;
    font.setPointSize(PROJECT_NAME_FONT_SIZE);
    p.setFont(font);
    p.drawText(LEFT_BORDER, PROJECT_NAME_Y_POS, QString::fromStdString(FoFiProjectInfoT::getProjectName()));

    font.setPointSize(DESCRIPTION_FONT_SIZE);
    p.setFont(font);
    p.drawText(LEFT_BORDER, DESCRIPTION_Y_POS, QString::fromStdString(FoFiProjectInfoT::getProjectDescription()));

    font.setPointSize(VERSION_FONT_SIZE);
    p.setFont(font);
    p.drawText(LEFT_BORDER, VERSION_NUMBER_Y_POS, QString::fromStdString(createFoFiVersionString()));

    font.setPointSize(DETAILS_FONT_SIZE);
    p.setFont(font);
    p.drawText(LEFT_BORDER, WEBSITE_Y_POS, QString::fromStdString(FoFiProjectInfoT::getWebsite()));
    p.drawText(LEFT_BORDER, CONTACT_Y_POS, QString::fromStdString(FoFiProjectInfoT::getMailAddress()));

    Q_UNUSED(event)
}
