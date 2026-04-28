#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/Riemann.png"));
    MainWindow window;
    window.setWindowIcon(QIcon(":/images/Riemann.png"));
    window.showMaximized();
    return app.exec();
}
