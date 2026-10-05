#include "custommenubar.h"
#include "customtitlebar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>

// ===========================================================================
//  Тема по умолчанию
// ===========================================================================
MenuBarTheme MenuBarTheme::defaultDark()
{
    return MenuBarTheme{};
}

// ===========================================================================
//  CustomMenuBar
// ===========================================================================
CustomMenuBar::CustomMenuBar(QWidget *parent)
    : QWidget(parent)
    , m_theme(MenuBarTheme::defaultDark())
{
    setFixedHeight(m_theme.height);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAutoFillBackground(false);

    m_lay = new QHBoxLayout(this);
    m_lay->setContentsMargins(4, 0, 4, 0);
    m_lay->setSpacing(2);
    m_lay->addStretch(1);

    applyTheme();
}

CustomMenuBar::~CustomMenuBar()
{
    qApp->removeEventFilter(this);
}

// ---------------------------------------------------------------------------
//  Тема
// ---------------------------------------------------------------------------
void CustomMenuBar::applyTheme()
{
    setFixedHeight(m_theme.height);

    setStyleSheet(QString(R"(
        CustomMenuBar {
            background-color: %1;
        }
    )").arg(m_theme.background.name()));

    for (QToolButton *b : m_buttons) {
        b->setStyleSheet(QString(R"(
            QToolButton {
                background: transparent;
                border: none;
                color: %1;
                font-family: "%2";
                font-size: %3px;
                padding: %4px %5px;
                border-radius: 2px;
            }
            QToolButton:hover {
                background-color: %6;
                color: %7;
            }
            QToolButton:pressed,
            QToolButton:checked,
            QToolButton[hovered="true"] {
                background-color: %8;
                color: %9;
            }
        )")
                             .arg(m_theme.text.name(),
                                  m_theme.fontFamily)
                             .arg(m_theme.fontSize)
                             .arg(m_theme.itemPaddingV)
                             .arg(m_theme.itemPaddingH)
                             .arg(m_theme.itemHover.name(),
                                  m_theme.textHover.name(),
                                  m_theme.itemActive.name(),
                                  m_theme.textActive.name()));
    }

    update();
}

void CustomMenuBar::setTheme(const MenuBarTheme &theme)
{
    m_theme = theme;
    applyTheme();
}

// ---------------------------------------------------------------------------
//  Создание кнопки пункта меню
// ---------------------------------------------------------------------------
QToolButton* CustomMenuBar::makeItemButton(const MenuBarItem &item)
{
    auto *btn = new QToolButton(this);
    btn->setText(item.title);
    btn->setAutoRaise(true);
    btn->setFocusPolicy(Qt::NoFocus);
    // DelayedPopup — нам не нужен встроенный механизм QToolButton, мы
    // управляем меню вручную через onButtonClicked/openMenuFor.
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    btn->setObjectName(item.objectName);

    // ★ НЕ вызываем setMenu() — управляем вручную
    // Меню запоминаем в m_btnToMenu (см. addItem)

    connect(btn, &QToolButton::clicked, this, &CustomMenuBar::onButtonClicked);

    return btn;
}

void CustomMenuBar::addItem(const MenuBarItem &item)
{
    QToolButton *btn = makeItemButton(item);
    m_btnToMenu.insert(btn, item.menu);
    // Вставляем перед stretch
    m_lay->insertWidget(m_lay->count() - 1, btn);
    m_buttons.append(btn);
    applyTheme();
}

void CustomMenuBar::addItem(const QString &title, QMenu *menu, const QString &objectName)
{
    MenuBarItem it;
    it.title = title;
    it.menu = menu;
    it.objectName = objectName;
    addItem(it);
}

void CustomMenuBar::clearItems()
{
    closeActiveMenu();
    m_btnToMenu.clear();
    for (QToolButton *b : m_buttons) {
        b->deleteLater();
    }
    m_buttons.clear();
}

QToolButton* CustomMenuBar::buttonFor(const QString &objectName) const
{
    for (QToolButton *b : m_buttons)
        if (b->objectName() == objectName) return b;
    return nullptr;
}

// ---------------------------------------------------------------------------
//  Логика открытия/переключения меню
// ---------------------------------------------------------------------------
void CustomMenuBar::onButtonClicked()
{
    auto *btn = qobject_cast<QToolButton*>(sender());
    if (!btn) return;

    // Повторный клик по той же кнопке — закрыть
    if (m_activeButton == btn) {
        closeActiveMenu();
        return;
    }
    openMenuFor(btn);
}

void CustomMenuBar::openMenuFor(QToolButton *btn)
{
    if (!btn) return;

    QMenu *menu = m_btnToMenu.value(btn, nullptr);
    if (!menu) return;

    // Если это то же меню — ничего не делаем
    if (m_activeMenu == menu) return;

    // Закрываем предыдущее (без сброса m_activeButton — см. ниже)
    if (m_activeMenu) {
        QMenu *old = m_activeMenu;
        m_activeMenu = nullptr;
        // отключаем возможные соединения
        disconnect(old, &QMenu::aboutToHide, this, nullptr);
        old->close();
    }

    m_activeButton = btn;

    emit itemAboutToOpen(btn->objectName());

    // Показываем меню под кнопкой
    const QPoint globalPos = btn->mapToGlobal(QPoint(0, btn->height()));
    menu->popup(globalPos);
    m_activeMenu = menu;

    // Следим за мышью и клавиатурой, пока меню открыто
    qApp->installEventFilter(this);

    // Когда меню закроется — сбросим состояние
    connect(menu, &QMenu::aboutToHide, this, [this, menu]{
        if (m_activeMenu == menu) {
            m_activeMenu = nullptr;
            m_activeButton = nullptr;
            qApp->removeEventFilter(this);
            update(); // перерисовать без подсветки
        }
    }, Qt::UniqueConnection);
}

void CustomMenuBar::closeActiveMenu()
{
    if (m_activeMenu) {
        QMenu *old = m_activeMenu;
        m_activeMenu = nullptr;
        disconnect(old, &QMenu::aboutToHide, this, nullptr);
        old->close();
    }
    m_activeButton = nullptr;
    qApp->removeEventFilter(this);
}

QToolButton* CustomMenuBar::neighborButton(QToolButton *cur, int dir) const
{
    if (m_buttons.isEmpty()) return nullptr;
    int idx = m_buttons.indexOf(cur);
    if (idx < 0) idx = 0;
    int next = idx + dir;
    if (next < 0) next = m_buttons.size() - 1;
    if (next >= m_buttons.size()) next = 0;
    return m_buttons.at(next);
}

// ---------------------------------------------------------------------------
//  Подсветка кнопок под курсором, пока меню открыто
// ---------------------------------------------------------------------------
void CustomMenuBar::updateHoverStates(const QPoint &globalPos)
{
    for (QToolButton *btn : m_buttons) {
        const QPoint local = btn->mapFromGlobal(globalPos);
        const bool under = btn->rect().contains(local);
        const bool wasHover = btn->property("hovered").toBool();
        if (wasHover != under) {
            btn->setProperty("hovered", under);
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
            btn->update();
        }
    }
}

// ---------------------------------------------------------------------------
//  eventFilter — сердце поведения
// ---------------------------------------------------------------------------
bool CustomMenuBar::eventFilter(QObject *watched, QEvent *event)
{
    // Обрабатываем только когда меню открыто
    if (!m_activeMenu) {
        return QWidget::eventFilter(watched, event);
    }

    switch (event->type()) {

        // === МЫШЬ: переключение по hover ===
    case QEvent::MouseMove: {
        auto *me = static_cast<QMouseEvent*>(event);
        const QPoint global = me->globalPosition().toPoint();

        // Обновим подсветку кнопок
        updateHoverStates(global);

        // Если курсор над другой кнопкой — переключаемся
        for (QToolButton *btn : m_buttons) {
            if (btn->rect().contains(btn->mapFromGlobal(global))) {
                if (btn != m_activeButton) {
                    openMenuFor(btn);
                }
                break;
            }
        }
        break;
    }

        // === КЛАВИАТУРА: ← / → — переключение между меню ===
    case QEvent::KeyPress: {
        auto *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right) {
            const int dir = (ke->key() == Qt::Key_Right) ? +1 : -1;
            QToolButton *next = neighborButton(m_activeButton, dir);
            if (next) {
                openMenuFor(next);
            }
            return true;   // съедаем событие, чтобы QMenu не обработал
        }
        break;
    }

    default: break;
    }

    return QWidget::eventFilter(watched, event);
}

// ---------------------------------------------------------------------------
//  Отрисовка фона
// ---------------------------------------------------------------------------
void CustomMenuBar::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.fillRect(rect(), m_theme.background);
    p.setPen(m_theme.borderBottom);
    p.drawLine(0, height() - 1, width(), height() - 1);
}

// ===========================================================================
//  TopBar
// ===========================================================================
TopBar::TopBar(QWidget *parent)
    : QWidget(parent)
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_titleBar = new CustomTitleBar(this);
    m_menuBar  = new CustomMenuBar(this);

    // Заголовок и менюбар не должны брать фокус
    m_titleBar->setFocusPolicy(Qt::NoFocus);
    m_menuBar->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::NoFocus);

    lay->addWidget(m_titleBar);
    lay->addWidget(m_menuBar);

    updateHeight();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TopBar::updateHeight()
{
    const int h = m_titleBar->height() + m_menuBar->height();
    setFixedHeight(h);
    if (auto *host = qobject_cast<QToolBar*>(parentWidget())) {
        host->setFixedHeight(h);
    }
}
