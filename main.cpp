#include "mainwindow.h"
#include <provisionmain.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    ProVisionMain *proVision = new ProVisionMain();
    proVision->startProVisionApp();

    //MainWindow w;
    //w.show();
    return a.exec();
}
