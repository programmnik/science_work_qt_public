#include "projectpanel.h"
#include "OpenGL/GLWidget.h"
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVector>

ProjectPanel::ProjectPanel(QMainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    m_glWidget = new GLWidget(this);
    mainLayout->addWidget(m_glWidget);

    setupDocks();
    setupFormulas();
    setupMathFunctions();
}

ProjectPanel::~ProjectPanel(){}

void ProjectPanel::setupDocks(){
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

    m_formulaDock = new QDockWidget("Ввод формулы", this);
    m_formulaDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_formulaDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_formulaDock->setStyleSheet(baseDockWidgetStyle);

    m_mathDock = new QDockWidget("Математические функции", this);
    m_mathDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    m_mathDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_mathDock->setStyleSheet(baseDockWidgetStyle);

    m_objectsDock = new QDockWidget("Объекты сцены", this);
    m_objectsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_objectsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_objectsDock->setStyleSheet(baseDockWidgetStyle);

    m_objectsPanel = new ObjectsPanelWidget(m_objectsDock);
    m_objectsDock->setWidget(m_objectsPanel);

    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_formulaDock);
    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_mathDock);
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_objectsDock);

    m_mainWindow->tabifyDockWidget(m_formulaDock, m_objectsDock);
}

void ProjectPanel::setupFormulas(){
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
    formulasLayout->setContentsMargins(10, 5, 10, 0);

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
                background-color: #353535;
                /* Эффект "вдавливания" при фокусе: инвертируем границы */
                border-top: 2px solid #252525;
                border-left: 2px solid #252525;
                border-bottom: 2px solid #4a4a4a;
                border-right: 2px solid #4a4a4a;

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
                background-color: #454444;
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
                border-top: 2px solid #0d7377;
                border-bottom: 2px solid #474747;
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
    m_formulaDock->setWidget(formulasWidget);
    // addDockWidget(Qt::LeftDockWidgetArea, m_formulaDock);
}

void ProjectPanel::setupMathFunctions(){
    m_mathPanel = new MathPanel(this);

    // Подключаемся к целевому полю — тому, которое сейчас активно
    // Например, ко всем QLineEdit формул:
    connect(m_mathPanel, &MathPanel::functionInserted, this, [](const QString &text) {
        qDebug() << "Вставлено:" << text;
    });

    m_mathDock->setWidget(m_mathPanel);

    // Настраиваем высоту дока, чтобы панель не занимала весь экран
    m_mathDock->setMinimumHeight(180);
    m_mathDock->setMaximumHeight(360);
}

void ProjectPanel::showDocks(){
    if (m_formulaDock) m_formulaDock->show();
    if (m_mathDock) m_mathDock->show();
    if (m_objectsDock) m_objectsDock->show();
}

void ProjectPanel::hideDocks(){
    if (m_formulaDock) m_formulaDock->hide();
    if (m_mathDock) m_mathDock->hide();
    if (m_objectsDock) m_objectsDock->hide();
}
