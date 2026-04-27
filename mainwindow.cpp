#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GLWidget.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GLWidget *glWidget = new GLWidget(this);
    setCentralWidget(glWidget);

    setWindowTitle(ScreenConfig::WINDOW_TITLE);
    resize(ScreenConfig::DEFAULT_WIDTH, ScreenConfig::DEFAULT_HEIGHT);
}

MainWindow::~MainWindow()
{
    delete ui;
}
