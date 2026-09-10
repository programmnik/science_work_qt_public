#ifndef PROJECTPANEL_H
#define PROJECTPANEL_H

#include <QWidget>
#include <QDockWidget>
#include "src/opengl/GLWidget.h"
#include "QMainWindow"
#include "src/ui/objectspanelwidget.h"

class ProjectPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectPanel(QMainWindow *mainWindow,QWidget *parent = nullptr);
    ~ProjectPanel();
    void showDocks();
    void hideDocks();
    GLWidget* getGLWidget() const { return m_glWidget; }

private:
    void setupDocks();
    void setupFormulas();
    void setupMathFunctions();

    QMainWindow *m_mainWindow;
    GLWidget *m_glWidget;
    QDockWidget *m_formulaDock;
    QDockWidget *m_mathDock;
    QDockWidget *m_objectsDock;
    ObjectsPanelWidget *m_objectsPanel;
};

#endif // PROJECTPANEL_H
