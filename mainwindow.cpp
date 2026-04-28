#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GLWidget.h"
#include "constants.h"
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVector>
#include <QPair>
#include <QToolBar>
#include <QToolButton>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *centralContainer = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(centralContainer);
    containerLayout->setContentsMargins(5, 5, 5, 5);

    GLWidget *glWidget = new GLWidget(centralContainer);
    containerLayout->addWidget(glWidget);

    setCentralWidget(centralContainer);

    QToolBar *leftToolBar = new QToolBar(this);
    leftToolBar->setOrientation(Qt::Vertical);
    leftToolBar->setFixedWidth(70);
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
            btn->setIconSize(QSize(32, 32));
            btn->setText(label);
        } else{
            btn->setText(QString("%1\n%2").arg(iconName, label));
        }

        btn->setStyleSheet(R"(
            QToolButton {
                background-color: #292857;
                border: none;
                color: white;
                font-size: 12px;
                font-weight: 400;
                min-height: 45px;
                min-width: 60px;
                padding: 3px 0px 0px 0px;
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

    // Создаём кнопки для тулбара
    QToolButton *menuBtn = createToolButton("menu", "Меню");
    QToolButton *projectBtn = createToolButton("П", "Проект");
    QToolButton *settingsBtn = createToolButton("Н", "Настройки");
    QToolButton *previewBtn = createToolButton("Пв", "Просмотр");
    QToolButton *createBtn = createToolButton("С", "Создать");
    QToolButton *openBtn = createToolButton("О", "Открыть");

    // Добавляем кнопки на тулбар
    leftToolBar->addWidget(menuBtn);
    leftToolBar->addWidget(projectBtn);
    leftToolBar->addWidget(settingsBtn);
    leftToolBar->addWidget(previewBtn);

    // Добавляем разделитель
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftToolBar->addWidget(spacer);

    leftToolBar->addWidget(createBtn);
    leftToolBar->addWidget(openBtn);

    // Добавляем тулбар на левую панель
    addToolBar(Qt::LeftToolBarArea, leftToolBar);

    QString baseDockWidgetStyle = R"(
        QDockWidget {
            titlebar-close-icon: url(:/images/noroot.png);
        }

        QDockWidget::title {
            background-color: rgb(60, 60, 60);
            color: rgb(218, 216, 227);
            border: none;
            border-bottom: 1px solid rgba(73, 75, 140, 0.9);
            padding: 5px 8px;
            text-align: left;
            font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
            font-size: 13px;
            font-weight: 500;
        }

        QDockWidget::close-button {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            width: 20px;
            height: 20px;
            margin: 2px;
            image: url(:/images/noroot.png);
            subcontrol-position: right;
            subcontrol-origin: padding;
        }

        QDockWidget::close-button:hover {
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 4px;
        }

        QDockWidget::close-button:pressed {
            background-color: rgba(255, 255, 255, 0.25);
            padding-top: 1px;
            padding-left: 1px;
        }
    )";

    QDockWidget *formulaDock = new QDockWidget("Ввод формулы", this);
    formulaDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    formulaDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);

    formulaDock->setStyleSheet(baseDockWidgetStyle);

    struct FormulaRow {
        QWidget *widget;
        QHBoxLayout *layout;
        QLabel *label;
        QLineEdit *input;
        QPushButton *button;
    };

    QWidget *formulasWidget = new QWidget();
    formulasWidget->setStyleSheet(R"(
        QWidget {
            background-color: #2b2b2b;
            font-family: 'URW Bookman', 'Microsoft YaHei', sans-serif;
        }

        /* Стиль для контейнера строки */
        QWidget[class="formulaRow"] {
            background-color: transparent;
            margin: 0px;
            padding: 0px;
        }
    )");

    QVBoxLayout *formulasLayout = new QVBoxLayout(formulasWidget);
    formulasLayout->setSpacing(0);
    formulasLayout->setContentsMargins(2, 5, 2, 0);

    QVector<FormulaRow> formulaRows;

    int numberOfFormulas = 5;
    for (int i = 0; i < numberOfFormulas; i++) {
        FormulaRow row;

        row.widget = new QWidget(formulasWidget);
        row.layout = new QHBoxLayout(row.widget);
        row.layout->setSpacing(0);
        row.layout->setContentsMargins(0, 0, 0, 0);
        row.input = new QLineEdit();
        row.input->setPlaceholderText("Введите формулу... например: sin(x) * cos(y)");

        QString baseInputStyle = R"(
            QLineEdit {
                background-color: #3c3c3c;
                border-bottom: 1px solid #474747;
                border-radius: 0px;
                padding: 0px 12px;
                color: #ffffff;
                font-size: 13px;
                selection-background-color: #0d7377;
                height: auto;
            }
            QLineEdit:focus {
                background-color: #404040;
                border: 2px solid #0d7377;
                border-right: none;
                padding: 7px 11px;
            }
            QLineEdit:hover {
                background-color: #3f3f3f;
                border: 1px solid #4a4a4a;
            }
            QLineEdit::placeholder {
                color: #8a8a8a;
                font-style: italic;
            }
        )";

        QString firstInputStyle = R"(
            QLineEdit {
                border-top-left-radius: 10px;
            }
        )";

        row.button = new QPushButton("Применить");

        QString baseButtonStyle = R"(
            QPushButton {
                background-color: #0d7377;
                border-bottom: 1px solid #474747;
                border-radius: 0px;
                padding: 0px 4px;
                color: white;
                font-family: 'URW Bookman', 'Microsoft YaHei', sans-serif;
                font-size: 13px;
                height: auto;
            }
            QPushButton:hover {
                background-color: #0f8b8f;
                border-color: #0c7074;
            }
            QPushButton:pressed {
                background-color: #0a5c5f;
                padding-top: 9px;
                padding-bottom: 7px;
                border-color: #084a4c;
            }
        )";

        QString firstButtonStyle = R"(
            QPushButton {
                border-top-right-radius: 10px;
            }
        )";


        if (i == 0){
            row.input->setStyleSheet(baseInputStyle + firstInputStyle);
            row.button->setStyleSheet(baseButtonStyle + firstButtonStyle);
        }
        else {
            row.input->setStyleSheet(baseInputStyle);
            row.button->setStyleSheet(baseButtonStyle);
        }

        row.layout->addWidget(row.input, 1);
        row.layout->addWidget(row.button);
        formulasLayout->addWidget(row.widget);

        formulaRows.append(row);

        connect(row.button, &QPushButton::clicked, [row]() {
            QString formula = row.input->text();
        });
    }

    formulasLayout->addStretch();
    formulaDock->setWidget(formulasWidget);
    addDockWidget(Qt::LeftDockWidgetArea, formulaDock);

    QDockWidget *mathDock = new QDockWidget("Математические функции", this);
    mathDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    mathDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    mathDock->setStyleSheet(baseDockWidgetStyle);

    QWidget *mathWidget = new QWidget();
    QVBoxLayout *mathLayout = new QVBoxLayout(mathWidget);

    QPushButton *sinBtn = new QPushButton("sin(x)");
    QPushButton *cosBtn = new QPushButton("cos(x)");
    QPushButton *tanBtn = new QPushButton("tan(x)");
    QPushButton *expBtn = new QPushButton("e^x");

    mathLayout->addWidget(sinBtn);
    mathLayout->addWidget(cosBtn);
    mathLayout->addWidget(tanBtn);
    mathLayout->addWidget(expBtn);
    mathLayout->addStretch();

    mathDock->setWidget(mathWidget);
    addDockWidget(Qt::BottomDockWidgetArea, mathDock);

    setWindowTitle(ScreenConfig::WINDOW_TITLE);
    resize(ScreenConfig::DEFAULT_WIDTH, ScreenConfig::DEFAULT_HEIGHT);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
