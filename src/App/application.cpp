#include "application.h"
#include "window/window.h"
#include "ApplicationConfig/AppConfig.h"

#include <QApplication>
#include <QIcon>

#include "Themes/dark/standarddark.h"

Application::Application(QObject* parent)
    : QObject(parent)
{
}

Application::~Application() = default;

void Application::initialize(QApplication& app)
{
    Q_INIT_RESOURCE(resources);
    Q_INIT_RESOURCE(config);

    QIcon icon(":/images/Riemann-multi.ico");
    app.setWindowIcon(icon);
    app.setStyleSheet(StandardDark::getGlobalStyle());

    setupConfig();
    setupWindow();

    if (m_window) {
        m_window->setWindowIcon(icon);
    }
}

void Application::run()
{
    if (!m_window) return;
}

void Application::setupConfig()
{
    // Передаем путь к ресурсному манифесту.
    // AppConfig сам развернет его в папку рядом с бинарником (build/config/)
    m_config = std::make_unique<AppConfig>(":/config/manifest.json");
    if (!m_config->load()) {
        qCritical() << "Не удалось загрузить конфигурацию!";
    }
}

void Application::setupWindow()
{
    m_window = std::make_unique<Window>();
}
