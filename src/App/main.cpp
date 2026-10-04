#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include "Themes/dark/standarddark.h"
#include <QImageReader>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    QIcon icon(":/images/Riemann-multi.ico");

    app.setWindowIcon(icon);
    app.setStyleSheet(StandardDark::getGlobalStyle());

    MainWindow window;
    window.setWindowIcon(icon);
    window.show();

    QTimer::singleShot(0, &window, &QWidget::showMaximized);

    return app.exec();
}
