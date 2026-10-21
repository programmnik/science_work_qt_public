// src/App/Application.h
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QApplication>
#include <memory>

class Window;
class AppConfig;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr);
    ~Application() override;

    /// Инициализация: настройка QApplication (иконка, стиль) и создание окна.
    void initialize(QApplication& app);

    /// Запустить приложение: показать главное окно.
    void run();

private:
    void setupConfig();
    void setupWindow();      // создаёт Window
    void setupChrome();      // WindowChrome
    void setupTopBar();      // TopBar
    void setupSideBar();     // SideBar
    void setupPanels();      // PanelStack
    void setupModeSwitch();  // ModeSwitcher
    void setupScene();       // SceneContext
    void connectAll();       // связывает сигналы/слоты между кирпичиками

    std::unique_ptr<AppConfig> m_config;
    std::unique_ptr<Window> m_window;

    // === Кирпичики (пока все закомментированы) ===
    // std::unique_ptr<WindowChrome>  m_chrome;
    // std::unique_ptr<TopBar>        m_topBar;
    // std::unique_ptr<SideBar>       m_sideBar;
    // std::unique_ptr<PanelStack>    m_panels;
    // std::unique_ptr<ModeSwitcher>  m_modeSwitch;
    // std::unique_ptr<SceneContext>  m_scene;
};

#endif // APPLICATION_H
