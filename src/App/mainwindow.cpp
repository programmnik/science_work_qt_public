#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/constants.h"
#include <QToolBar>
#include <QFile>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_centralStack = new QStackedWidget(this);

    m_projectPanel = new ProjectPanel(this, m_centralStack);
    m_projectPanel->setMinimumSize(0, 0);
    m_projectPanel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_centralStack->addWidget(m_projectPanel);

    m_previewPanel = new PreviewPanel(m_centralStack);
    m_previewPanel->setMinimumSize(0, 0);
    m_previewPanel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_centralStack->addWidget(m_previewPanel);

    m_settingsPanel = new SettingsPanel(this);
    m_settingsPanel->setMinimumSize(0, 0);
    m_settingsPanel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_centralStack->addWidget(m_settingsPanel);

    m_menuPanel = new MenuPanel(this);
    m_menuPanel->setMinimumSize(0, 0);
    m_menuPanel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_centralStack->addWidget(m_menuPanel);

    setCentralWidget(m_centralStack);

    setupLeftTool();
    setupModeButton();

    setWindowTitle(ScreenConfig::WINDOW_TITLE);
    resize(ScreenConfig::DEFAULT_WIDTH, ScreenConfig::DEFAULT_HEIGHT);

    // centralWidget()->setFocus();
    GLWidget* glWidget = m_projectPanel->getGLWidget();
    glWidget->setFocus(Qt::MouseFocusReason);
}

void MainWindow::setupLeftTool(){
    QToolBar *leftToolBar = new QToolBar(this);
    leftToolBar->setOrientation(Qt::Vertical);
    leftToolBar->setFixedWidth(80);
    leftToolBar->setMovable(false);
    leftToolBar->setFloatable(false);
    leftToolBar->setAllowedAreas(Qt::LeftToolBarArea);

    leftToolBar->setStyleSheet(R"(
        QToolBar {
            background-color: #292857;
            border: none;
            spacing: 0px;
            padding: 0px;
        }
        QToolBar::separator {
            background-color: #3d3d3d;
            height: 1px;
            margin: 0px;
        }
    )");

    auto createToolButton = [](const QString &iconName, const QString &label, bool checkable)
        -> QToolButton*
    {
        QToolButton *btn = new QToolButton;
        btn->setText(label);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setMinimumHeight(40);
        btn->setMaximumWidth(90);
        btn->setCheckable(checkable);

        if(!iconName.isEmpty() && QFile::exists(QString(":/images/%1.png").arg(iconName))){
            btn->setIcon(QIcon(QString(":/images/%1.png").arg(iconName)));
            btn->setIconSize(QSize(40, 40));
            btn->setText(label);
        } else{
            btn->setText(QString("%1\n%2").arg(iconName, label));
        }

        btn->setStyleSheet(R"(
            QToolButton {
                background-color: #292857;
                border: none;
                color: white;
                font-size: 10px;
                font-weight: 600;
                min-height: 45px;
                min-width: 60px;
                padding: 10px 0px 0px 0px;
            }
            QToolButton:hover {
                background-color: #0d7377;
            }
            QToolButton:pressed {
                background-color: #0a5c5f;
            }

            QToolButton::icon {
                margin-top: 8px;
            }

            QToolButton:checked {
                background-color: #0d7377;
                border-left: 3px solid #00ffcc;
            }
        )");

        return btn;
    };

    QToolButton *menuBtn = createToolButton("menu", "Меню", true);
    QToolButton *projectBtn = createToolButton("project", "Проект", true);
    QToolButton *settingsBtn = createToolButton("settings", "Настройки", true);
    QToolButton *previewBtn = createToolButton("preview", "Превью", true);
    QToolButton *createBtn = createToolButton("create_new", "Создать", false);
    QToolButton *openBtn = createToolButton("open", "Открыть", false);

    m_toolButtons.append({menuBtn, "menu"});
    m_toolButtons.append({projectBtn, "project"});
    m_toolButtons.append({settingsBtn, "settings"});
    m_toolButtons.append({previewBtn, "preview"});

    connect(menuBtn, &QToolButton::clicked, this, &MainWindow::onMenuClicked);
    connect(projectBtn, &QToolButton::clicked, this, &MainWindow::onProjectClicked);
    connect(settingsBtn, &QToolButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(previewBtn, &QToolButton::clicked, this, &MainWindow::onPreviewClicked);
    connect(createBtn, &QToolButton::clicked, this, &MainWindow::onCreateClicked);
    connect(openBtn, &QToolButton::clicked, this, &MainWindow::onOpenClicked);

    leftToolBar->addWidget(menuBtn);
    leftToolBar->addWidget(projectBtn);
    leftToolBar->addWidget(settingsBtn);
    leftToolBar->addWidget(previewBtn);

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftToolBar->addWidget(spacer);

    leftToolBar->addWidget(createBtn);
    leftToolBar->addWidget(openBtn);

    addToolBar(Qt::LeftToolBarArea, leftToolBar);

    setActiveButton(projectBtn);
}

void MainWindow::setActiveButton(QToolButton *activeBtn){
    for (const QPair<QToolButton*, QString> &pair : m_toolButtons){
        QToolButton* btn = pair.first;
        QString iconImage = pair.second;
        btn->setChecked(btn == activeBtn);
        if(btn == activeBtn){
            if (!iconImage.isEmpty() && QFile::exists(QString(":/images/%1-colored.png").arg(iconImage))){
                btn->setIcon(QIcon(QString(":/images/%1-colored.png").arg(iconImage)));
                btn->setIconSize(QSize(40, 40));
            }
        } else {
            if (!iconImage.isEmpty() && QFile::exists(QString(":/images/%1.png").arg(iconImage))){
                btn->setIcon(QIcon(QString(":/images/%1.png").arg(iconImage)));
                btn->setIconSize(QSize(40, 40));
            }
        }
    }
}

void MainWindow::onMenuClicked(){
    setActiveButton(qobject_cast<QToolButton*>(sender()));
    m_projectPanel->hideDocks();
    m_centralStack->setCurrentWidget(m_menuPanel);
    if (m_modeButton) m_modeButton->hide();
}

void MainWindow::onProjectClicked(){
    setActiveButton(qobject_cast<QToolButton*>(sender()));
    m_centralStack->setCurrentWidget(m_projectPanel);
    m_projectPanel->showDocks();
    if (m_modeButton) m_modeButton->show();
    if (m_modeButton) m_modeButton->raise();
}

void MainWindow::onSettingsClicked(){
    setActiveButton(qobject_cast<QToolButton*>(sender()));
    m_projectPanel->hideDocks();
    m_centralStack->setCurrentWidget(m_settingsPanel);
    if (m_modeButton) m_modeButton->hide();
}

void MainWindow::onPreviewClicked(){
    setActiveButton(qobject_cast<QToolButton*>(sender()));
    m_projectPanel->hideDocks();
    m_centralStack->setCurrentWidget(m_previewPanel);
    if (m_modeButton) m_modeButton->hide();
}

void MainWindow::onCreateClicked(){

}

void MainWindow::onOpenClicked(){

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Обработка F11 для полноэкранного режима (без панели задач)
    if (event->key() == Qt::Key_F11) {
        if (isFullScreen()) {
            showMaximized();  // Возвращаемся в развернутое окно
        } else {
            showFullScreen(); // Переходим в полноэкранный режим
        }
        return;  // Важно: не вызывать QMainWindow::keyPressEvent дальше
    }

    // Обработка F3 для скрытия панелей
    if (event->key() == Qt::Key_F3) {
        if(m_hiddenMenuBar != m_hiddenLeftTool){
            m_hiddenMenuBar = false;
            m_hiddenLeftTool = false;
        }
        else{
            m_hiddenMenuBar = !m_hiddenMenuBar;
            m_hiddenLeftTool = !m_hiddenLeftTool;
        }

        if (menuBar()) {
            menuBar()->setVisible(!m_hiddenMenuBar);
        }

        QList<QToolBar*> toolBars = findChildren<QToolBar*>();
        for (QToolBar *toolBar : toolBars) {
            if (toolBar->allowedAreas() == Qt::LeftToolBarArea) {
                toolBar->setVisible(!m_hiddenLeftTool);
            }
        }
        return;
    }

    // Передаем остальные события дальше
    QMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModeButton()
{
    m_modeButton = new ToggleButton(centralWidget());
    m_modeButton->setFixedSize(50, 50);
    m_modeButton->setBorderRadius(10);
    m_modeButton->setFontSize(12);

    // Настраиваем состояния
    ToggleButton::State state3D;
    state3D.text = "2D";
    state3D.color = QColor(13, 115, 119, 220);
    state3D.hoverColor = QColor(13, 115, 119, 255);
    state3D.pressedColor = QColor(10, 90, 95, 200);
    state3D.borderColor = QColor(255, 255, 255, 50);
    state3D.textColor = Qt::white;
    state3D.userData = "3D";

    ToggleButton::State state2D;
    state2D.text = "3D";
    state2D.color = QColor(200, 100, 50, 220);
    state2D.hoverColor = QColor(220, 120, 60, 255);
    state2D.pressedColor = QColor(180, 80, 40, 200);
    state2D.borderColor = QColor(255, 200, 100, 80);
    state2D.textColor = Qt::white;
    state2D.userData = "2D";

    m_modeButton->addState("3D", state3D);
    m_modeButton->addState("2D", state2D);
    m_modeButton->setState("3D");

    // Позиционируем
    int margin = 20;
    m_modeButton->move(centralWidget()->width() - m_modeButton->width() - margin, margin);

    // Подключаем сигнал
    connect(m_modeButton, &ToggleButton::stateChanged,
            this, [this](const QString& oldState, const QString& newState) {
        Q_UNUSED(oldState);
        bool is2D = (newState == "2D");

        // Переключаем режим в GLWidget
        if (m_projectPanel) {
            GLWidget* glWidget = m_projectPanel->getGLWidget();
            if (glWidget) {
                glWidget->setMode2D(is2D);
                glWidget->setFocus(Qt::MouseFocusReason);
            }
        }
    });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_modeButton && centralWidget()) {
        int margin = 20;
        m_modeButton->move(centralWidget()->width() - m_modeButton->width() - margin, margin);
        m_modeButton->raise();
    }
}

void MainWindow::on_action_13_triggered()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        centralWidget()->setFocus();
        showFullScreen();
    }
}


void MainWindow::on_action_14_triggered()
{
    if(m_hiddenMenuBar != m_hiddenLeftTool){
        m_hiddenMenuBar = false;
        m_hiddenLeftTool = false;
    }
    else{
        m_hiddenMenuBar = !m_hiddenMenuBar;
        m_hiddenLeftTool = !m_hiddenLeftTool;
    }

    if (menuBar()) {
        menuBar()->setVisible(!m_hiddenMenuBar);
    }

    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    for (QToolBar *toolBar : toolBars) {
        if (toolBar->allowedAreas() == Qt::LeftToolBarArea) {
            toolBar->setVisible(!m_hiddenLeftTool);
        }
    }
}


void MainWindow::on_action_15_triggered()
{
    m_hiddenMenuBar = !m_hiddenMenuBar;

    if (menuBar()) {
        menuBar()->setVisible(!m_hiddenMenuBar);
    }
}


void MainWindow::on_action_16_triggered()
{
    m_hiddenLeftTool = !m_hiddenLeftTool;

    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    for (QToolBar *toolBar : toolBars) {
        if (toolBar->allowedAreas() == Qt::LeftToolBarArea) {
            toolBar->setVisible(!m_hiddenLeftTool);
        }
    }
}


void MainWindow::on_action_21_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Выход", "Вы действительно хотите выйти?"
                                  , QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes){
        QCoreApplication::quit();
    }
}

