#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include <QToolBar>
#include <QToolButton>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_centralStack = new QStackedWidget(this);

    m_projectPaneel = new ProjectPanel(this, m_centralStack);
    m_centralStack->addWidget(m_projectPaneel);

    setCentralWidget(m_centralStack);

    setupLeftTool();

    setWindowTitle(ScreenConfig::WINDOW_TITLE);
    resize(ScreenConfig::DEFAULT_WIDTH, ScreenConfig::DEFAULT_HEIGHT);

    centralWidget()->setFocus();
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

    auto createToolButton = [](const QString &iconName, const QString &label) -> QToolButton* {
        QToolButton *btn = new QToolButton;
        btn->setText(label);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setMinimumHeight(40);
        btn->setMaximumWidth(90);

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
        )");

        return btn;
    };

    QToolButton *menuBtn = createToolButton("menu", "Меню");
    QToolButton *projectBtn = createToolButton("project", "Проект");
    QToolButton *settingsBtn = createToolButton("settings", "Настройки");
    QToolButton *previewBtn = createToolButton("view", "Превью");
    QToolButton *createBtn = createToolButton("create_new", "Создать");
    QToolButton *openBtn = createToolButton("open", "Открыть");

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
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        if (isFullScreen()) {
            showNormal();
        } else {
            centralWidget()->setFocus();
            showFullScreen();
        }
    }

    if (event->key() == Qt::Key_F3) {
        m_hiddenPanels = !m_hiddenPanels;

        if (menuBar()) {
            menuBar()->setVisible(!m_hiddenPanels);
        }

        QList<QToolBar*> toolBars = findChildren<QToolBar*>();
        for (QToolBar *toolBar : toolBars) {
            if (toolBar->allowedAreas() == Qt::LeftToolBarArea) {
                toolBar->setVisible(!m_hiddenPanels);
            }
        }
    }

    QMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    m_hiddenPanels = !m_hiddenPanels;

    if (menuBar()) {
        menuBar()->setVisible(!m_hiddenPanels);
    }

    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    for (QToolBar *toolBar : toolBars) {
        if (toolBar->allowedAreas() == Qt::LeftToolBarArea) {
            toolBar->setVisible(!m_hiddenPanels);
        }
    }
}

