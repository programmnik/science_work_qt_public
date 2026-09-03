#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QToolButton>
#include <QList>
#include <QPair>

#include "projectpanel.h"
#include "previewpanel.h"
#include "src/opengl/GLWidget.h"

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

    void on_action_15_triggered();

    void on_action_16_triggered();

    void on_action_21_triggered();

private:
    Ui::MainWindow *ui;
    void keyPressEvent(QKeyEvent *event) override;

    void setupLeftTool();
    void setActiveButton(QToolButton *activeBtn);
    void onMenuClicked();
    void onProjectClicked();
    void onSettingsClicked();
    void onPreviewClicked();
    void onCreateClicked();
    void onOpenClicked();

    QStackedWidget *m_centralStack;
    ProjectPanel *m_projectPanel;
    QWidget *m_settingsPanel;
    PreviewPanel *m_previewPanel;
    QWidget *m_menuPanel;
    QList<QPair<QToolButton*, QString>> m_toolButtons;

    bool m_hiddenMenuBar = false;
    bool m_hiddenLeftTool = false;
};

#endif // MAINWINDOW_H
