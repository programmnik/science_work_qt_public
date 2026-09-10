#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include "src/themes/dark/standarddark.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/Riemann.png"));
    app.setStyleSheet(StandardDark::getGlobalStyle());

    MainWindow window;
    window.setWindowIcon(QIcon(":/images/Riemann.png"));
    window.show();

    QTimer::singleShot(10, &window, &QWidget::showMaximized);

    return app.exec();
}
