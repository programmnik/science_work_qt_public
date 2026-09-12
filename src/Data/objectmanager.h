#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <memory>
#include "sceneobject.h"

class ObjectManager : public QObject
{
    Q_OBJECT

public:
    static ObjectManager& instance();

    void addObject(std::shared_ptr<SceneObject> object);
    void removeObject(SceneObject* object);
    void removeObjectAt(int index);
    void clearAll();

    QVector<std::shared_ptr<SceneObject>> getAllObjects() const { return m_objects; }
    QVector<std::shared_ptr<SceneObject>> getObjectsByType(SceneObject::ObjectType type) const;
    QMap<SceneObject::ObjectType, QVector<std::shared_ptr<SceneObject>>> getGroupedObjects() const;

    int getObjectCount() const { return m_objects.size(); }
    std::shared_ptr<SceneObject> getObjectAt(int index) const;

signals:
    void objectAdded(std::shared_ptr<SceneObject> object);
    void objectRemoved(std::shared_ptr<SceneObject> object);
    void objectUpdated(std::shared_ptr<SceneObject> object);
    void allObjectsCleared();

private:
    ObjectManager() = default;
    ~ObjectManager() = default;
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

    QVector<std::shared_ptr<SceneObject>> m_objects;
};

#endif // OBJECTMANAGER_H
