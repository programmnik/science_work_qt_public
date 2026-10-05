#pragma once

#include <QWidget>
#include <QColor>
#include <QString>
#include <QList>
#include <QHash>
#include <QToolBar>
#include "customtitlebar.h"

class QHBoxLayout;
class QToolButton;
class QMenu;

// ---------------------------------------------------------------------------
//  Описание одного пункта меню.
// ---------------------------------------------------------------------------
struct MenuBarItem
{
    QString title;              // видимый текст: "Файл", "Правка", ...
    QMenu  *menu = nullptr;     // выпадающее меню (владелец — тот, кто передал)
    QString objectName;         // для поиска/скрытия
};

// ---------------------------------------------------------------------------
//  Тема оформления меню-бара.
// ---------------------------------------------------------------------------
struct MenuBarTheme
{
    int     height        = 24;
    QColor  background    = QColor("#2A2A2E");
    QColor  borderBottom  = QColor("#1A1A1C");
    QColor  text          = QColor("#A0A0A8");
    QColor  textHover     = QColor("#00B4D8");
    QColor  textActive    = QColor("#E8E8EA");
    QColor  itemHover     = QColor("#3A3A44");
    QColor  itemActive    = QColor("#33333A");
    QString fontFamily    = "Inter";
    int     fontSize      = 11;
    int     itemPaddingH  = 10;   // горизонтальные отступы текста
    int     itemPaddingV  = 3;

    static MenuBarTheme defaultDark();
};

// ---------------------------------------------------------------------------
//  Кастомный меню-бар.
// ---------------------------------------------------------------------------
class CustomMenuBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomMenuBar(QWidget *parent = nullptr);
    ~CustomMenuBar() override;

    void setTheme(const MenuBarTheme &theme);
    const MenuBarTheme& theme() const { return m_theme; }

    // Добавление/удаление пунктов
    void addItem(const MenuBarItem &item);
    void addItem(const QString &title, QMenu *menu, const QString &objectName = {});
    void clearItems();

    // Найти кнопку пункта по objectName (для скрытия/показа)
    QToolButton* buttonFor(const QString &objectName) const;

signals:
    void itemAboutToOpen(const QString &objectName);
    void menuClosed();

protected:
    void paintEvent(QPaintEvent *e) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onButtonClicked();

private:
    QToolButton* makeItemButton(const MenuBarItem &item);
    void applyTheme();

    // --- логика меню ---
    void openMenuFor(QToolButton *btn);            // показать меню под кнопкой
    void closeActiveMenu();                        // скрыть текущее меню
    QToolButton* neighborButton(QToolButton *cur, int dir) const;
    void updateHoverStates(const QPoint &globalPos); // подсветка кнопок под курсором

    MenuBarTheme                m_theme;
    QHBoxLayout                *m_lay = nullptr;
    QList<QToolButton*>         m_buttons;
    QHash<QToolButton*, QMenu*> m_btnToMenu;

    QToolButton                *m_activeButton = nullptr;
    QMenu                      *m_activeMenu   = nullptr;
};

// ---------------------------------------------------------------------------
//  Контейнер «верх окна»: заголовок + менюбар.
// ---------------------------------------------------------------------------
class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);

    CustomTitleBar* titleBar() const { return m_titleBar; }
    CustomMenuBar*  menuBar()  const { return m_menuBar;  }

    // Пересчитать высоту (если менялись темы заголовка/меню)
    void updateHeight();

private:
    CustomTitleBar *m_titleBar = nullptr;
    CustomMenuBar  *m_menuBar  = nullptr;
};
