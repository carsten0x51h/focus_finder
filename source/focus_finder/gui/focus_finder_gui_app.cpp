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

#include <QApplication>
#include <QString>
#include <QFile>

#include "../common/include/focus_finder_logic.h"

#include "include/main_window.h"


int main(int argc, char *argv[]) {
    FocusFinderLogicT::init();

    QApplication application(argc, argv);


    // See https://stackoverflow.com/questions/4448236/how-could-qt-apply-style-from-an-external-qt-stylesheet-file
    QFile styleSheetFile(":/res/style.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    application.setStyleSheet(styleSheet);

    // We may pass the Logic here... however, since it is currently static,
    // it can be accessed from everywhere in the app without passing it everywhere...
    MainWindow mainWindow;
    mainWindow.show();

    int rc = QApplication::exec();

    FocusFinderLogicT::close();

    return rc;
}
