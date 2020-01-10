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

#include "include/main_window.h"
#include <stdio.h>

#include <QApplication>
#include <QString>
#include <QFile>

#include "../common/include/logging.h"
#include "../common/include/image.h"

int main(int argc, char *argv[])
{
    logging::trivial::severity_level sev = logging::trivial::debug;
    LoggingT::init(sev, true /*console*/, true /*log file*/);

    QApplication application(argc, argv);


    // See https://stackoverflow.com/questions/4448236/how-could-qt-apply-style-from-an-external-qt-stylesheet-file
    QFile File(":/res/style.qss");
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());

    application.setStyleSheet(styleSheet);

    
    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
