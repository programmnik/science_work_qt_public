#ifndef APP_WINDOW_WINDOW_H
#define APP_WINDOW_WINDOW_H

#include <QMainWindow>

/**
 * Window — базовое окно приложения.
 *
 * Ответственность (SRP):
 *   - frameless-режим (без системной рамки и менюбара),
 *   - заголовок и иконка,
 *   - стартовые размеры,
 *   - показ.
 */
class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent = nullptr);
    ~Window() override;

    /// Показать окно и развернуть на весь экран.
    void showMaximizedDeferred();

private:
    void setupFrameless();
    void setupTitle();
    void setupIcon();
    void setupSize();
};

#endif // APP_WINDOW_WINDOW_H
