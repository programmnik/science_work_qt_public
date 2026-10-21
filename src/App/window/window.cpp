#include "window.h"

#include <QIcon>
#include <QTimer>
#include <QWidget>

#include "Core/constants.h"

Window::Window(QWidget* parent) : QMainWindow(parent)
{
    setupFrameless();
    setupTitle();
    setupIcon();
    showMaximizedDeferred();
}

Window::~Window() = default;

void Window::setupFrameless()
{
    // Убираем системный менюбар и рамку.
    setMenuBar(nullptr);
    setWindowFlag(Qt::FramelessWindowHint, true);
}

void Window::setupTitle()
{
    setWindowTitle(ScreenConfig::WINDOW_TITLE);
}

void Window::setupIcon()
{
    QIcon icon(":/images/Riemann-multi.ico");
    setWindowIcon(icon);
}

void Window::setupSize()
{
    resize(ScreenConfig::DEFAULT_WIDTH, ScreenConfig::DEFAULT_HEIGHT);
}

void Window::showMaximizedDeferred()
{
    show();
    QTimer::singleShot(0, this, &QWidget::showMaximized);
}
