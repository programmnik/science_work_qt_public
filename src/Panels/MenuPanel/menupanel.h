#ifndef MENUPANEL_H
#define MENUPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMap>

// ============ Карточка проекта ============
class ProjectListItem : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectListItem(const QString &name, const QString &description,
                             const QString &modified, const QString &thumbnailColor,
                             bool starred = false, QWidget *parent = nullptr);
    bool isStarred() const { return m_starred; }
signals:
    void openRequested();
    void starToggled(bool starred);
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    QFrame *m_thumbnail;
    QPushButton *m_starButton;
    bool m_hovered = false;
    bool m_starred = false;
};

// ============ Вкладка (Последние / Избранные) ============
class SegmentedTab : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentedTab(const QStringList &items, QWidget *parent = nullptr);
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
signals:
    void currentChanged(int index);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    QStringList m_items;
    int m_currentIndex = 0;
    int m_hoveredIndex = -1;
    QList<QRect> m_itemRects;
    void recalcRects();
};

// ============ Основная панель меню ============
class MenuPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MenuPanel(QWidget *parent = nullptr);
    ~MenuPanel();

signals:
    void createProjectRequested();
    void openProjectRequested();
    void saveProjectRequested();
    void exportProjectRequested();
    void importProjectRequested();
    void recentProjectRequested(const QString &name);
    void accountRequested();
    void aboutRequested();
    void helpRequested();
    void onlineResourcesRequested();
    void documentationRequested();

private slots:
    void onActionClicked(QListWidgetItem *item);
    void onAccountClicked(QListWidgetItem *item);
    void onMoreClicked(QListWidgetItem *item);
    void onProjectFilterChanged(int index);

private:
    void setupUI();
    QWidget* createHeader();
    QWidget* createSidebar();
    QWidget* createFooter();

    QWidget* createProjectsPage();
    QWidget* createAccountPage();
    QWidget* createLearnPage();

    QWidget* buildProjectList(bool onlyStarred);

    // Централизованное переключение активной группы и страницы
    void selectGroup(QListWidget *active, int row);

    QListWidget *m_actionList  = nullptr;
    QListWidget *m_accountList = nullptr;
    QListWidget *m_moreList    = nullptr;
    QStackedWidget *m_contentStack = nullptr;

    QLabel *m_pageTitle    = nullptr;
    QLabel *m_pageSubtitle = nullptr;

    SegmentedTab *m_projectTabs = nullptr;
    QStackedWidget *m_projectsStack = nullptr;

    QPushButton *m_createBtn = nullptr;
    QPushButton *m_openBtn   = nullptr;
    QPushButton *m_saveBtn   = nullptr;
    QPushButton *m_exportBtn = nullptr;
    QPushButton *m_importBtn = nullptr;

    bool m_updatingSelection = false;
};

#endif // MENUPANEL_H
