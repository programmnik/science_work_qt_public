#ifndef PROJECTPANEL_H
#define PROJECTPANEL_H

#include <QWidget>
#include <QDockWidget>
#include "GLWidget.h"
#include "QMainWindow"

class ProjectPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectPanel(QMainWindow *mainWindow,QWidget *parent = nullptr);
    ~ProjectPanel();

private:
    void setupDocks();
    void setupFormulas();
    void setupMathFunctions();

    QMainWindow *m_mainWindow;
    GLWidget *m_glWidget;
    QDockWidget *m_formulaDock;
    QDockWidget *m_mathDock;
};

#endif // PROJECTPANEL_H
