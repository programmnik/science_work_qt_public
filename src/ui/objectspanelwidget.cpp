#include "objectspanelwidget.h"
#include <QDebug>

ObjectsPanelWidget::ObjectsPanelWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
    refreshList();
}

ObjectsPanelWidget::~ObjectsPanelWidget() {}

void ObjectsPanelWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QLabel* titleLabel = new QLabel("Объекты сцены");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #e8e8e8;");
    mainLayout->addWidget(titleLabel);

    m_countLabel = new QLabel("Объектов: 0");
    m_countLabel->setStyleSheet("color: #8888a0; font-size: 12px;");
    mainLayout->addWidget(m_countLabel);

    m_objectList = new QListWidget();
    m_objectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_objectList->setStyleSheet(R"(
        QListWidget {
            background-color: #1a1a2e;
            border: 1px solid #2a2a4a;
            border-radius: 8px;
            color: #e0e0e0;
            font-size: 12px;
            padding: 4px;
        }
        QListWidget::item {
            padding: 6px 10px;
            border-radius: 4px;
            margin: 2px 0px;
        }
        QListWidget::item:selected {
            background-color: #0d7377;
            color: white;
        }
        QListWidget::item:hover {
            background-color: #2a2a4a;
        }
    )");
    mainLayout->addWidget(m_objectList);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);

    m_visibilityBtn = new QPushButton("👁 Скрыть");
    m_visibilityBtn->setStyleSheet(getButtonStyle());
    buttonLayout->addWidget(m_visibilityBtn);

    m_removeBtn = new QPushButton("✕ Удалить");
    m_removeBtn->setStyleSheet(getButtonStyle() + "QPushButton { color: #ff6b6b; }");
    buttonLayout->addWidget(m_removeBtn);

    m_refreshBtn = new QPushButton("⟳ Обновить");
    m_refreshBtn->setStyleSheet(getButtonStyle());
    buttonLayout->addWidget(m_refreshBtn);

    mainLayout->addLayout(buttonLayout);

    setStyleSheet("background-color: #1a1a2e; border-radius: 12px;");
}

QString ObjectsPanelWidget::getButtonStyle() const
{
    return R"(
        QPushButton {
            background-color: #2a2a4a;
            border: none;
            border-radius: 6px;
            color: #e0e0e0;
            padding: 6px 12px;
            font-size: 11px;
        }
        QPushButton:hover {
            background-color: #3a3a5a;
        }
        QPushButton:pressed {
            background-color: #1a1a3a;
        }
    )";
}

void ObjectsPanelWidget::connectSignals()
{
    connect(m_removeBtn, &QPushButton::clicked,
            this, &ObjectsPanelWidget::onRemoveSelectedClicked);
    connect(m_visibilityBtn, &QPushButton::clicked,
            this, &ObjectsPanelWidget::onToggleVisibilityClicked);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &ObjectsPanelWidget::refreshList);

    auto& manager = ObjectManager::instance();
    connect(&manager, &ObjectManager::objectAdded,
            this, &ObjectsPanelWidget::onObjectAdded);
    connect(&manager, &ObjectManager::objectRemoved,
            this, &ObjectsPanelWidget::onObjectRemoved);
    connect(&manager, &ObjectManager::allObjectsCleared,
            this, &ObjectsPanelWidget::onAllObjectsCleared);
}

void ObjectsPanelWidget::refreshList()
{
    m_objectList->clear();

    auto& manager = ObjectManager::instance();
    auto grouped = manager.getGroupedObjects();

    for (auto it = grouped.begin(); it != grouped.end(); ++it) {
        SceneObject::ObjectType type = it.key();
        const auto& objects = it.value();

        if (objects.isEmpty()) continue;

        QString typeName = m_typeNames.value(type, "Другое");
        QListWidgetItem* header = new QListWidgetItem(
            QString("─── %1 (%2) ───").arg(typeName).arg(objects.size())
            );
        header->setFlags(header->flags() & ~Qt::ItemIsSelectable);
        header->setForeground(QColor(100, 100, 150));
        m_objectList->addItem(header);

        for (const auto& obj : objects) {
            QString icon = getTypeIcon(type);
            QString desc = obj->getDescription();

            QListWidgetItem* item = new QListWidgetItem(
                QString("%1 %2").arg(icon).arg(desc)
                );
            item->setData(Qt::UserRole, QVariant::fromValue(obj.get()));

            if (!obj->isVisible()) {
                item->setForeground(QColor(100, 100, 100));
                item->setText(item->text() + " (скрыт)");
            }

            m_objectList->addItem(item);
        }
    }

    updateObjectCount();
}

void ObjectsPanelWidget::onObjectAdded(std::shared_ptr<SceneObject> object)
{
    Q_UNUSED(object);
    refreshList();
}

void ObjectsPanelWidget::onObjectRemoved(std::shared_ptr<SceneObject> object)
{
    Q_UNUSED(object);
    refreshList();
}

void ObjectsPanelWidget::onAllObjectsCleared()
{
    refreshList();
}

void ObjectsPanelWidget::onRemoveSelectedClicked()
{
    auto selected = m_objectList->selectedItems();
    auto& manager = ObjectManager::instance();

    for (auto* item : selected) {
        SceneObject* obj = item->data(Qt::UserRole).value<SceneObject*>();
        if (obj) {
            manager.removeObject(obj);
        }
    }
}

void ObjectsPanelWidget::onToggleVisibilityClicked()
{
    auto selected = m_objectList->selectedItems();
    if (selected.isEmpty()) return;

    bool allVisible = true;
    for (auto* item : selected) {
        SceneObject* obj = item->data(Qt::UserRole).value<SceneObject*>();
        if (obj && !obj->isVisible()) {
            allVisible = false;
            break;
        }
    }

    for (auto* item : selected) {
        SceneObject* obj = item->data(Qt::UserRole).value<SceneObject*>();
        if (obj) {
            obj->setVisible(!allVisible);
        }
    }

    refreshList();
}

void ObjectsPanelWidget::updateObjectCount()
{
    int count = ObjectManager::instance().getObjectCount();
    m_countLabel->setText(QString("Объектов: %1").arg(count));
}

QString ObjectsPanelWidget::getTypeIcon(SceneObject::ObjectType type)
{
    switch (type) {
    case SceneObject::TypeSphere:    return "🔵";
    case SceneObject::TypePlane:     return "⬜";
    case SceneObject::TypeProjection:return "🟠";
    case SceneObject::TypePoint:     return "🟡";
    default:                         return "⬛";
    }
}
