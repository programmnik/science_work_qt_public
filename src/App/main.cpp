#include <QApplication>

// === Новая архитектура (по умолчанию) ===
#include "application.h"

// === Старая архитектура (на время миграции) ===
// Раскомментируй, если нужно вернуться к старому MainWindow.
// #include "mainwindow.h"
// #include <QTimer>
// #include "Themes/dark/standarddark.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ============ НОВЫЙ ПУТЬ ============
    Application application;
    application.initialize(app);
    application.run();
    // ===================================

    // ============ СТАРЫЙ ПУТЬ (на время миграции) ============
    // Q_INIT_RESOURCE(resources);
    // QIcon icon(":/images/Riemann-multi.ico");
    // app.setWindowIcon(icon);
    // app.setStyleSheet(StandardDark::getGlobalStyle());
    //
    // MainWindow window;
    // window.setWindowIcon(icon);
    // window.show();
    // QTimer::singleShot(0, &window, &QWidget::showMaximized);
    // =======================================================

    return app.exec();
}
