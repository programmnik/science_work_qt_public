#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QStackedWidget>

#include "projectpanel.h"
#include "GLWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_13_triggered();

    void on_action_14_triggered();

private:
    Ui::MainWindow *ui;
    void keyPressEvent(QKeyEvent *event) override;

    void setupLeftTool();

    QStackedWidget *m_centralStack;
    ProjectPanel *m_projectPaneel;
    GLWidget *m_glWidget;

    bool m_hiddenPanels = false;
};

#endif // MAINWINDOW_H
