#ifndef OBJECTSPANELWIDGET_H
#define OBJECTSPANELWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include "Data/objectmanager.h"
#include "Data/sceneobject.h"

class ObjectsPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectsPanelWidget(QWidget *parent = nullptr);
    ~ObjectsPanelWidget();

    void refreshList();

private slots:
    void onObjectAdded(std::shared_ptr<SceneObject> object);
    void onObjectRemoved(std::shared_ptr<SceneObject> object);
    void onAllObjectsCleared();
    void onRemoveSelectedClicked();
    void onToggleVisibilityClicked();

private:
    void setupUI();
    void connectSignals();
    void updateObjectCount();
    QString getTypeIcon(SceneObject::ObjectType type);
    QString getButtonStyle() const;

    QListWidget* m_objectList;
    QLabel* m_countLabel;
    QPushButton* m_removeBtn;
    QPushButton* m_visibilityBtn;
    QPushButton* m_refreshBtn;

    QMap<SceneObject::ObjectType, QString> m_typeNames = {
        {SceneObject::TypeSphere, "Сфера"},
        {SceneObject::TypePlane, "Плоскость"},
        {SceneObject::TypeProjection, "Проекция"},
        {SceneObject::TypePoint, "Точка"}
    };
};

#endif // OBJECTSPANELWIDGET_H
