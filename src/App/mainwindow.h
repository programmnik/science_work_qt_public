#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QToolButton>
#include <QList>
#include <QPair>
#include <QResizeEvent>

#include "customtitlebar.h"
#include "custommenubar.h"
#include "Panels/ProjectPanel/projectpanel.h"
#include "Panels/PreviewPanel/previewpanel.h"
#include "Panels/SettingsPanel/settingspanel.h"
#include "Panels/MenuPanel/menupanel.h"
#include "OpenGL/GLWidget.h"
#include "UI/ToggleButton/togglebutton.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void buildMenus();
    void onMenuClosed();
    void on_action_13_triggered();
    void on_action_14_triggered();

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
    void setupModeButton();

    QStackedWidget *m_centralStack;
    ProjectPanel *m_projectPanel;
    SettingsPanel *m_settingsPanel;
    PreviewPanel *m_previewPanel;
    MenuPanel *m_menuPanel;
    QList<QPair<QToolButton*, QString>> m_toolButtons;
    ToggleButton* m_modeButton = nullptr;

    bool m_hiddenMenuBar = false;
    bool m_hiddenLeftTool = false;

    std::function<void()> m_repositionModeButton;
    TopBar *m_topBar = nullptr;
};

#endif // MAINWINDOW_H
