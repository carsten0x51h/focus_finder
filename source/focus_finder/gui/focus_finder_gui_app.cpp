
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
