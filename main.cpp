#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/Riemann.png"));

    MainWindow window;
    window.setWindowIcon(QIcon(":/images/Riemann.png"));
    window.show();

    QTimer::singleShot(10, &window, &QWidget::showMaximized);

    return app.exec();
}
