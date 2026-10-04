#include "mathpanel.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QApplication>
#include <QTimer>

// ==================================================================
// Палитра — минималистичная, тёмная, с синевато-фиолетовыми акцентами
// ==================================================================
namespace MathPalette {
// Фоны
const QString BgRoot        = "#232328";   // общий фон панели
const QString BgSection     = "#2a2a31";   // блоки/секции
const QString BgInset       = "#1e1e23";   // вложенные области (поля, клавиатура)
const QString BgButton      = "#33333b";   // обычная кнопка
const QString BgButtonHover = "#3d3d47";
const QString BgButtonAccent= "#5b4bdb";   // акцентная (сине-фиолет)
const QString BgButtonAccHov= "#6b5be6";

// Акценты (сине-фиолетовые, приглушённые)
const QString Accent        = "#5b4bdb";
const QString AccentSoft    = "#4a3fb0";
const QString AccentText    = "#9b8bff";
const QString AccentBorder  = "rgba(91,75,219,0.35)";

// Текст
const QString TextPrimary   = "#e6e6ee";
const QString TextSecondary = "#a0a0b0";
const QString TextMuted     = "#70707e";

// Границы / сепараторы
const QString Border        = "rgba(255,255,255,0.06)";
const QString Separator     = "rgba(91,75,219,0.18)";  // тусклый сине-фиолет
const QString SeparatorStrong = "rgba(91,75,219,0.3)";
}

// ==================================================================
// SegmentTab
// ==================================================================
SegmentTab::SegmentTab(const QStringList &items, QWidget *parent)
    : QWidget(parent), m_items(items)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
}

void SegmentTab::setOrientation(Orientation o)
{
    if (m_orient == o) return;
    m_orient = o;
    recalcRects();
    updateGeometry();
    update();
}

void SegmentTab::recalcRects()
{
    m_itemRects.clear();
    const int n = m_items.size();
    if (n == 0) return;

    if (m_orient == Orientation::Horizontal) {
        const int w = width() / n;
        for (int i = 0; i < n; ++i) {
            const int x = i * w;
            const int wi = (i == n - 1) ? (width() - x) : w;
            m_itemRects.append(QRect(x, 0, wi, height()));
        }
    } else {
        const int h = height() / n;
        for (int i = 0; i < n; ++i) {
            const int y = i * h;
            const int hi = (i == n - 1) ? (height() - y) : h;
            m_itemRects.append(QRect(0, y, width(), hi));
        }
    }
}

void SegmentTab::resizeEvent(QResizeEvent *)
{
    recalcRects();
}

QSize SegmentTab::sizeHint() const
{
    QFont f("DejaVu Sans", m_fontPt);
    QFontMetrics fm(f);

    const int textH = fm.height();

    if (m_orient == Orientation::Horizontal) {
        int w = 0;
        for (const QString &s : m_items)
            w += fm.horizontalAdvance(s) + 24;   // padding по бокам
        return QSize(w, qMax(m_itemHeight, textH + 8));
    } else {
        int w = 0;
        for (const QString &s : m_items)
            w = qMax(w, fm.horizontalAdvance(s) + 24);
        return QSize(qMax(m_itemWidth, w),
                     textH * m_items.size() + 8 * m_items.size());
    }
}

QSize SegmentTab::minimumSizeHint() const
{
    QFont f("DejaVu Sans", m_fontPt);
    QFontMetrics fm(f);
    const int textH = fm.height();

    if (m_orient == Orientation::Horizontal) {
        return QSize(0, textH + 2);
    } else {
        return QSize(40, textH + 2);
    }
}

void SegmentTab::paintEvent(QPaintEvent *)
{
    using namespace MathPalette;
    if (m_itemRects.size() != m_items.size()) recalcRects();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Фон контейнера
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect().adjusted(0, 0, -1, -1), 6, 6);
    p.fillPath(bgPath, QColor(BgInset));

    // Активная вкладка
    if (m_currentIndex >= 0 && m_currentIndex < m_itemRects.size()) {
        QRect activeRect = m_itemRects[m_currentIndex].adjusted(2, 2, -2, -2);
        QPainterPath activePath;
        activePath.addRoundedRect(activeRect, 4, 4);
        p.fillPath(activePath, QColor(Accent));
    }

    // Текст
    for (int i = 0; i < m_items.size(); ++i) {
        QColor textColor;
        if (i == m_currentIndex)      textColor = QColor("#ffffff");
        else if (i == m_hoveredIndex) textColor = QColor(AccentText);
        else                          textColor = QColor(TextMuted);

        QFont f("DejaVu Sans", m_fontPt,
                i == m_currentIndex ? QFont::DemiBold : QFont::Normal);
        p.setFont(f);
        p.setPen(textColor);

        const QRect &r = m_itemRects[i];

        // Автосокращение по ширине
        QString text = m_items[i];
        QFontMetrics fm(f);
        const int avail = r.width() - 12;
        if (fm.horizontalAdvance(text) > avail) {
            text = fm.elidedText(text, Qt::ElideRight, avail);
        }

        p.drawText(r, Qt::AlignCenter, text);
    }
}

void SegmentTab::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < m_itemRects.size(); ++i) {
        if (m_itemRects[i].contains(event->pos())) {
            setCurrentIndex(i);
            break;
        }
    }
}

void SegmentTab::mouseMoveEvent(QMouseEvent *event)
{
    int newHover = -1;
    for (int i = 0; i < m_itemRects.size(); ++i) {
        if (m_itemRects[i].contains(event->pos())) { newHover = i; break; }
    }
    if (newHover != m_hoveredIndex) { m_hoveredIndex = newHover; update(); }
}

void SegmentTab::leaveEvent(QEvent *)
{
    m_hoveredIndex = -1;
    update();
}

void SegmentTab::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.size() || index == m_currentIndex) return;
    m_currentIndex = index;
    update();
    emit currentChanged(index);
}

// ==================================================================
// MathButton
// ==================================================================
MathButton::MathButton(const QString &label, const QString &insertText,
                       bool accent, QWidget *parent)
    : QPushButton(label, parent)
    , m_insertText(insertText)
    , m_accent(accent)
{
    setFixedHeight(28);
    setMinimumWidth(38);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);

    int openParen = insertText.indexOf('(');
    int closeParen = insertText.indexOf(')', openParen + 1);
    if (openParen >= 0 && closeParen == openParen + 1) {
        m_cursorOffset = openParen + 1;
    }

    setAccent(accent);
}

void MathButton::setAccent(bool accent)
{
    using namespace MathPalette;
    m_accent = accent;
    QString bg = accent ? BgButtonAccent : BgButton;
    QString bgHover = accent ? BgButtonAccHov : BgButtonHover;
    QString border = accent ? Accent : Border;
    QString color = accent ? "#ffffff" : TextPrimary;

    setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 5px;
            color: %3;
            font-family: 'DejaVu Sans', sans-serif;
            font-size: 11px;
            font-weight: 500;
            padding: 0 6px;
        }
        QPushButton:hover {
            background-color: %4;
            border-color: %5;
        }
        QPushButton:pressed {
            background-color: %6;
            padding-top: 1px;
        }
    )").arg(bg, border, color, bgHover, Accent, AccentSoft));
}

// ==================================================================
// MathSection
// ==================================================================
MathSection::MathSection(QWidget *parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
}

void MathSection::setContent(QWidget *w)
{
    if (m_content) {
        m_layout->removeWidget(m_content);
        m_content->deleteLater();
    }
    m_content = w;
    m_layout->addWidget(w);
}

void MathSection::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

// ==================================================================
// MathPanel
// ==================================================================
MathPanel::MathPanel(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet(QString("background-color: %1;").arg(MathPalette::BgRoot));

    setMinimumSize(0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);

    setupUI();
}

MathPanel::~MathPanel() = default;

bool MathPanel::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        if (auto *w = qobject_cast<QWidget*>(watched)) {
            // запоминаем любой виджет, получивший фокус, кроме наших кнопок и панели
            if (!qobject_cast<MathButton*>(w) && w != this && !isAncestorOf(w)) {
                m_lastFocusedWidget = w;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MathPanel::restoreTargetFocus()
{
    if (m_lastFocusedWidget) {
        m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
    } else if (m_targetEdit) {
        m_targetEdit->setFocus(Qt::OtherFocusReason);
    }
}

void MathPanel::setupUI()
{
    qApp->installEventFilter(this);

    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(0, 0, 0, 0);
    m_rootLayout->setSpacing(0);

    // Верхний бар с горизонтальным SegmentTab (narrow-режим)
    m_mainTabsBar = createMainTabs();
    m_rootLayout->addWidget(m_mainTabsBar);

    // Контейнер, куда войдёт либо stack, либо [вертикальный SegmentTab + stack]
    m_contentRow = new QHBoxLayout();
    m_contentRow->setContentsMargins(0, 0, 0, 0);
    m_contentRow->setSpacing(0);

    m_mainStack = new QStackedWidget();
    m_mainStack->setStyleSheet(QString("background-color: %1;").arg(MathPalette::BgRoot));

    m_mainStack->addWidget(createFunctionsSection());   // 0
    m_mainStack->addWidget(createCalculatorSection());  // 1
    m_mainStack->addWidget(createKeyboardSection());    // 2

    m_contentRow->addWidget(m_mainStack, 1);

    m_rootLayout->addLayout(m_contentRow, 1);

    // Анимация
    m_opacityEffect = new QGraphicsOpacityEffect(m_mainStack);
    m_mainStack->setGraphicsEffect(m_opacityEffect);
    m_fadeAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeAnim->setDuration(150);
    m_fadeAnim->setEasingCurve(QEasingCurve::InOutQuad);

    // Изначально — narrow
    applyLayoutMode(false);
}

// ============ Главные вкладки ============
QWidget* MathPanel::createMainTabs()
{
    using namespace MathPalette;

    QWidget *bar = new QWidget();
    bar->setFixedHeight(32);
    bar->setStyleSheet(QString(
                           "background-color: %1;"
                           "border-bottom: 1px solid %2;"
                           ).arg(BgSection, Separator));

    QHBoxLayout *barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(8, 2, 8, 2);
    barLayout->setSpacing(0);

    // Создаём один SegmentTab. Позже мы будем менять его ориентацию
    // и переставлять между bar и m_contentRow.
    m_mainTabs = new SegmentTab({"Функции", "Калькулятор", "Клавиатура"});
    m_mainTabs->setFontSize(8);
    m_mainTabs->setOrientation(SegmentTab::Orientation::Horizontal);
    connect(m_mainTabs, &SegmentTab::currentChanged,
            this, &MathPanel::onMainSectionChanged);

    barLayout->addWidget(m_mainTabs);
    return bar;
}

// ============ Переключение режимов ============
void MathPanel::applyLayoutMode(bool wide)
{
    if (wide == m_wideMode && !m_wideMode == m_mainTabsBar->isVisible())
        return; // ничего не изменилось

    m_wideMode = wide;

    if (wide) {
        // WIDE: бар сверху скрыт, SegmentTab переезжает в левую часть m_contentRow
        m_mainTabsBar->setVisible(false);

        // Забираем SegmentTab из бара
        if (m_mainTabs->parentWidget() != this) {
            m_mainTabs->setParent(this);
        }
        m_mainTabs->setOrientation(SegmentTab::Orientation::Vertical);
        m_mainTabs->setFixedWidth(90);
        m_mainTabs->setMinimumHeight(0);
        m_mainTabs->setMaximumHeight(QWIDGETSIZE_MAX);
        m_mainTabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

        // Вставляем в начало m_contentRow
        m_contentRow->insertWidget(0, m_mainTabs, 0);

        // Обновляем стиль бара — оставим фон, но без границы
        m_mainTabsBar->setStyleSheet(QString(
                                         "background-color: %1;"
                                         ).arg(MathPalette::BgRoot));
    } else {
        // NARROW: SegmentTab возвращается в бар, вертикальный режим выключаем
        if (m_mainTabs->parentWidget() != m_mainTabsBar) {
            // Убираем из m_contentRow и вставляем в bar
            m_contentRow->removeWidget(m_mainTabs);
            m_mainTabs->setParent(m_mainTabsBar);

            // Найдём layout бара и добавим туда
            if (auto *bl = qobject_cast<QHBoxLayout*>(m_mainTabsBar->layout())) {
                bl->addWidget(m_mainTabs);
            }
        }

        m_mainTabs->setOrientation(SegmentTab::Orientation::Horizontal);
        m_mainTabs->setMinimumWidth(0);
        m_mainTabs->setMaximumWidth(QWIDGETSIZE_MAX);
        m_mainTabs->setFixedWidth(QWIDGETSIZE_MAX);
        m_mainTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        m_mainTabsBar->setStyleSheet(QString(
                                         "background-color: %1;"
                                         "border-bottom: 1px solid %2;"
                                         ).arg(MathPalette::BgSection, MathPalette::Separator));

        m_mainTabsBar->setVisible(true);
    }

    m_mainTabs->updateGeometry();
    m_mainTabs->update();
}

void MathPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const bool wide = (width() > 600);
    if (wide != m_wideMode) {
        applyLayoutMode(wide);
    }

    // Пересчёт строк сеток функций по высоте
    const int newRows = (height() > 200) ? 3 : 2;
    if (newRows != m_functionsRows) {
        m_functionsRows = newRows;
        rebuildFunctionsSection();
    }
}

void MathPanel::rebuildFunctionsSection()
{
    if (!m_mainStack) return;

    const int idx = 0; // «Функции»
    QWidget *old = m_mainStack->widget(idx);
    if (!old) return;

    // Сохраняем текущий индекс главного стека
    const int savedMainIndex = m_mainStack->currentIndex();
    // Сохраняем текущую категорию внутри «Функций»
    const int savedCatIndex = m_categoryStack ? m_categoryStack->currentIndex() : 0;

    QWidget *fresh = createFunctionsSection();

    // Заменяем старую страницу
    m_mainStack->insertWidget(idx, fresh);
    m_mainStack->removeWidget(old);
    old->deleteLater();

    // Восстанавливаем индексы
    if (m_mainStack->count() > savedMainIndex)
        m_mainStack->setCurrentIndex(savedMainIndex);

    if (m_categoryStack && savedCatIndex < m_categoryStack->count())
        m_categoryStack->setCurrentIndex(savedCatIndex);
}

QWidget* MathPanel::buildFunctionsPage(const QList<FuncGroup> &groups)
{
    using namespace MathPalette;

    // --- прокрутка ---
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet(QString(
                              "QScrollArea { border: none; background: %1; }"
                              "QScrollBar:vertical { background: transparent; width: 5px; margin: 4px 0; }"
                              "QScrollBar::handle:vertical { background: %2; border-radius: 2px; min-height: 20px; }"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
                              "QScrollBar:horizontal { background: transparent; height: 5px; margin: 0 4px; }"
                              "QScrollBar::handle:horizontal { background: %2; border-radius: 2px; min-width: 20px; }"
                              "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
                              ).arg(BgRoot, SeparatorStrong));

    QWidget *container = new QWidget();
    container->setStyleSheet(QString("background-color: %1;").arg(BgRoot));

    QVBoxLayout *outer = new QVBoxLayout(container);
    outer->setContentsMargins(12, 10, 12, 10);
    outer->setSpacing(0);

    // --- контейнер с максимальной шириной + центрирование ---
    QWidget *centerHost = new QWidget();
    centerHost->setMaximumWidth(900);
    centerHost->setMinimumWidth(0);
    centerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *centerLay = new QHBoxLayout(centerHost);
    centerLay->setContentsMargins(0, 0, 0, 0);
    centerLay->setSpacing(24);   // отступ между группами

    // --- число строк в сетке по высоте ---
    // Если доступная высота большая — 3 строки × 2 колонки,
    // если маленькая — 2 строки × 3 колонки.
    const int rows = m_functionsRows;

    for (const FuncGroup &g : groups) {
        // ----- блок группы: заголовок + сетка -----
        QWidget *groupBlock = new QWidget();
        QVBoxLayout *groupLay = new QVBoxLayout(groupBlock);
        groupLay->setContentsMargins(0, 0, 0, 0);
        groupLay->setSpacing(6);

        // заголовок группы
        if (!g.title.isEmpty()) {
            QLabel *lbl = new QLabel(g.title.toUpper());
            lbl->setStyleSheet(QString(
                                   "color: %1;"
                                   "font-family: 'DejaVu Sans', sans-serif;"
                                   "font-size: 9px;"
                                   "font-weight: 600;"
                                   "letter-spacing: 1.0px;"
                                   "background: transparent;"
                                   "padding: 2px 0;"
                                   ).arg(TextMuted));
            groupLay->addWidget(lbl);
        }

        // сетка кнопок: rows фиксировано, cols = ceil(n / rows)
        QGridLayout *grid = new QGridLayout();
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setHorizontalSpacing(4);
        grid->setVerticalSpacing(4);

        const int n = g.items.size();
        const int cols = qMax(1, (n + rows - 1) / rows);

        for (int i = 0; i < n; ++i) {
            const auto &p = g.items[i];
            MathButton *btn = new MathButton(p.first, p.second);
            btn->setMinimumWidth(0);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                insertToTarget(btn->insertText(), btn->cursorOffset());
                QTimer::singleShot(0, this, [this]() {
                    if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
                });
            });

            // раскладка по столбцам: сначала сверху вниз, потом вправо
            const int r = i % rows;
            const int c = i / rows;
            grid->addWidget(btn, r, c);
        }

        // добиваем пустыми ячейками, чтобы колонки были равной ширины
        const int totalCells = rows * cols;
        for (int i = n; i < totalCells; ++i) {
            QWidget *spacer = new QWidget();
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spacer->setFixedHeight(28);
            grid->addWidget(spacer, i % rows, i / rows);
        }

        for (int c = 0; c < cols; ++c)
            grid->setColumnStretch(c, 1);

        groupLay->addLayout(grid);
        groupLay->addStretch(); // прижимаем содержимое вверх, если высоты больше

        centerLay->addWidget(groupBlock, 0, Qt::AlignTop);
    }

    // центрирование centerHost в outer
    QHBoxLayout *hosterLay = new QHBoxLayout();
    hosterLay->setContentsMargins(0, 0, 0, 0);
    hosterLay->setSpacing(0);
    hosterLay->addStretch(1);
    hosterLay->addWidget(centerHost, 1000000);
    hosterLay->addStretch(1);

    outer->addStretch(1);
    outer->addLayout(hosterLay);
    outer->addStretch(1);

    scroll->setWidget(container);
    return scroll;
}

QWidget* MathPanel::createFunctionsSection()
{
    using namespace MathPalette;

    QWidget *section = new QWidget();
    section->setStyleSheet(QString("background-color: %1;").arg(BgRoot));

    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // ---- панель подкатегорий (как было) ----
    m_categoryNames = {
        "Тригонометрия", "Гиперболические", "Анализ", "Статистика",
        "Числа", "Геометрия", "Распределения", "Логика"
    };

    QWidget *catBar = new QWidget();
    catBar->setFixedHeight(32);
    catBar->setStyleSheet(QString(
                              "background-color: %1;"
                              "border-bottom: 1px solid %2;"
                              ).arg(BgSection, Separator));

    QHBoxLayout *catLayout = new QHBoxLayout(catBar);
    catLayout->setContentsMargins(8, 4, 8, 4);
    catLayout->setSpacing(0);

    m_categoryTabs = new SegmentTab(m_categoryNames);
    m_categoryTabs->setFontSize(10);
    connect(m_categoryTabs, &SegmentTab::currentChanged,
            this, &MathPanel::onCategoryChanged);

    catLayout->addWidget(m_categoryTabs);
    layout->addWidget(catBar);

    // ---- стек подкатегорий ----
    m_categoryStack = new QStackedWidget();
    m_categoryStack->setStyleSheet(QString("background-color: %1;").arg(BgRoot));

    // 1 — Тригонометрия
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Тригонометрические функции", {
                                           {"sin","sin()"}, {"cos","cos()"}, {"tan","tan()"},
                                           {"csc","csc()"}, {"sec","sec()"}, {"cot","cot()"}
                                       }},
        { "Обратные тригонометрические", {
                                            {"sin⁻¹","asin()"}, {"cos⁻¹","acos()"}, {"tan⁻¹","atan()"},
                                            {"csc⁻¹","acsc()"}, {"sec⁻¹","asec()"}, {"cot⁻¹","acot()"}
                                        }}
    }));

    // 2 — Гиперболические
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Гиперболические функции", {
                                        {"sinh","sinh()"}, {"cosh","cosh()"}, {"tanh","tanh()"},
                                        {"csch","csch()"}, {"sech","sech()"}, {"coth","coth()"}
                                    }},
        { "Обратные гиперболические", {
                                         {"sinh⁻¹","asinh()"}, {"cosh⁻¹","acosh()"}, {"tanh⁻¹","atanh()"}
                                     }}
    }));

    // 3 — Анализ
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Элементарные", {
                             {"eˣ","exp()"}, {"ln","ln()"}, {"log","log()"},
                             {"logₐ","log_a()"}, {"√","sqrt()"}, {"∛","cbrt()"}
                         }},
        { "Дифференцирование и суммы", {
                                          {"d/dx","d/dx"}, {"f′","f'"}, {"∫","∫()dx"},
                                          {"Σ","Σ()"}, {"Π","Π()"}, {"lim","lim"}
                                      }}
    }));

    // 4 — Статистика
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Основные", {
                         {"mean","mean()"}, {"median","median()"},
                         {"min","min()"}, {"max","max()"},
                         {"stdev","stdev()"}, {"var","var()"}
                     }},
        { "Дополнительные", {
                               {"quartile","quartile()"}, {"quantile","quantile()"},
                               {"mad","mad()"}, {"cov","cov()"},
                               {"corr","corr()"}, {"count","count()"}
                           }}
    }));

    // 5 — Числа
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Целочисленные", {
                              {"gcd","gcd()"}, {"lcm","lcm()"}, {"mod","mod"},
                              {"ceil","ceil()"}, {"floor","floor()"}, {"round","round()"}
                          }},
        { "Комбинаторика и знак", {
                                     {"sign","sign()"}, {"nCr","nCr()"}, {"nPr","nPr()"},
                                     {"n!","!"}, {"|x|","abs()"}, {"⌊x⌋","floor()"}
                                 }}
    }));

    // 6 — Геометрия
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Объекты", {
                        {"polygon","polygon()"}, {"distance","distance()"},
                        {"midpoint","midpoint()"}, {"∠","angle"},
                        {"°","°"}, {"π","π"}
                    }},
        { "Символы", {
                        {"Δ","Δ"}, {"⊥","⊥"}, {"∥","∥"},
                        {"△","△"}, {"○","○"}, {"□","□"}
                    }}
    }));

    // 7 — Распределения
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Распределения", {
                              {"normaldist","normaldist()"}, {"tdist","tdist()"},
                              {"chisqdist","chisqdist()"}, {"uniformdist","uniformdist()"},
                              {"binomialdist","binomialdist()"}, {"poissondist","poissondist()"}
                          }},
        { "Функции плотности и тесты", {
                                          {"pdf","pdf()"}, {"cdf","cdf()"},
                                          {"inversecdf","inversecdf()"}, {"random","random()"},
                                          {"ztest","ztest()"}, {"ttest","ttest()"}
                                      }}
    }));

    // 8 — Логика
    m_categoryStack->addWidget(buildFunctionsPage({
        { "Логические операторы", {
                                     {"and","and"}, {"or","or"}, {"not","not"},
                                     {"=","="}, {"≠","≠"}, {"<","<"}
                                 }},
        { "Сравнения и множества", {
                                      {">",">"}, {"≤","≤"}, {"≥","≥"},
                                      {"∈","∈"}, {"∉","∉"}, {"∅","∅"}
                                  }}
    }));

    layout->addWidget(m_categoryStack, 1);

    m_categoryStack->setCurrentIndex(0);
    return section;
}

// ============ Раздел: Калькулятор ============
QWidget* MathPanel::createCalculatorSection()
{
    using namespace MathPalette;

    QWidget *section = new QWidget();
    section->setStyleSheet(QString("background-color: %1;").arg(BgRoot));

    // Внешний слой — вертикальное центрирование
    QVBoxLayout *outer = new QVBoxLayout(section);
    outer->setContentsMargins(0, 6, 0, 6);
    outer->setSpacing(0);

    outer->addStretch(1); // пустота сверху

    // Контейнер, который держит три блока и ограничен 1200 px
    QWidget *centerHost = new QWidget();
    centerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    centerHost->setMaximumWidth(1200);
    centerHost->setMinimumWidth(0);

    QHBoxLayout *centerLay = new QHBoxLayout(centerHost);
    centerLay->setContentsMargins(12, 0, 12, 0);
    centerLay->setSpacing(24);

    // ---------- вспомогательная фабрика кнопок ----------
    auto makeCalcBtn = [](const QString &label, bool accent = false) {
        MathButton *btn = new MathButton(label, label, accent);
        btn->setMinimumHeight(20);
        btn->setMaximumHeight(36);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumWidth(0);
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background-color: %1;
                border: 1px solid %2;
                border-radius: 6px;
                color: %3;
                font-family: 'DejaVu Sans', sans-serif;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: %4;
                border-color: %5;
            }
            QPushButton:pressed {
                background-color: %6;
                padding-top: 1px;
            }
        )").arg(accent ? BgButtonAccent : BgButton,
                                    accent ? Accent : Border,
                                    accent ? "#ffffff" : TextPrimary,
                                    accent ? BgButtonAccHov : BgButtonHover,
                                    Accent,
                                    AccentSoft));
        return btn;
    };

    // ---------- универсальная сборка блока через QGridLayout ----------
    // Каждый блок описывается списком строк фиксированной ширины `cols`.
    // Пустые строки `QString()` превращаются в невидимые спейсеры.
    // `accentItems` — множество меток, которые рисуются акцентным цветом.
    auto buildGridBlock = [&](const QList<QStringList> &rowsData,
                              int cols,
                              int hSpacing,
                              int vSpacing,
                              const QSet<QString> &accentItems = {}) -> QWidget*
    {
        QWidget *w = new QWidget();
        QGridLayout *gl = new QGridLayout(w);
        gl->setContentsMargins(0, 0, 0, 0);
        gl->setHorizontalSpacing(hSpacing);
        gl->setVerticalSpacing(vSpacing);

        for (int r = 0; r < rowsData.size(); ++r) {
            const QStringList &row = rowsData[r];
            for (int c = 0; c < cols; ++c) {
                const QString item = (c < row.size()) ? row[c] : QString();

                if (item.isEmpty()) {
                    // пустая ячейка — невидимый спейсер, чтобы сохранить сетку
                    QWidget *spacer = new QWidget();
                    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    spacer->setFixedHeight(36);
                    gl->addWidget(spacer, r, c);
                    continue;
                }

                MathButton *btn = makeCalcBtn(item, accentItems.contains(item));
                connect(btn, &QPushButton::clicked, this, [this, btn]() {
                    insertToTarget(btn->insertText(), btn->cursorOffset());
                    QTimer::singleShot(0, this, [this]() {
                        if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
                    });
                });
                gl->addWidget(btn, r, c);
            }
        }

        // все колонки равномерно растягиваются
        for (int c = 0; c < cols; ++c)
            gl->setColumnStretch(c, 1);

        // все строки одинаковой высоты, растягиваются равномерно
        for (int r = 0; r < rowsData.size(); ++r)
            gl->setRowStretch(r, 1);

        return w;
    };

    // ---------- левый блок ----------
    QList<QStringList> leftRows = {
        {"x","y", "x²", "xʸ"},
        {"(", ")","<",">"},
        {"x⁻¹", ",","<=",">="},
        {"π","e","|x|","√"}
    };
    QWidget *leftBlock = buildGridBlock(leftRows, /*cols*/4,
                                        /*hSpacing*/6, /*vSpacing*/6);

    // ---------- центральный блок (цифры) ----------
    QList<QStringList> centerRows = {
        {"7","8","9","÷"},
        {"4","5","6","×"},
        {"1","2","3","−"},
        {"0",".","=","+"}
    };
    QWidget *centerBlock = buildGridBlock(centerRows, /*cols*/4,
                                          /*hSpacing*/6, /*vSpacing*/6,
                                          {"÷","×","−","+","=","."});

    // ---------- правый блок ----------
    // Укладываем в ту же сетку 2×3 (2 колонки, 3 строки).
    // ⌫ и ↵ занимают по две колонки, ← и → — по одной.
    QWidget *rightBlock = new QWidget();
    QGridLayout *rightGrid = new QGridLayout(rightBlock);
    rightGrid->setContentsMargins(0, 0, 0, 0);
    rightGrid->setHorizontalSpacing(6);
    rightGrid->setVerticalSpacing(6);

    // ⌫ — на всю ширину (2 колонки)
    {
        MathButton *btn = makeCalcBtn("⌫", true);
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            insertToTarget(btn->insertText(), btn->cursorOffset());
            QTimer::singleShot(0, this, [this]() {
                if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
            });
        });
        rightGrid->addWidget(btn, 0, 0, 1, 2);
    }
    // ← и → — по одной колонке
    {
        MathButton *left  = makeCalcBtn("←");
        MathButton *right = makeCalcBtn("→");
        connect(left,  &QPushButton::clicked, this, [this, left]() {
            insertToTarget(left->insertText(), left->cursorOffset());
            QTimer::singleShot(0, this, [this]() {
                if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
            });
        });
        connect(right, &QPushButton::clicked, this, [this, right]() {
            insertToTarget(right->insertText(), right->cursorOffset());
            QTimer::singleShot(0, this, [this]() {
                if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
            });
        });
        rightGrid->addWidget(left,  1, 0);
        rightGrid->addWidget(right, 1, 1);
    }
    // ↵ — на всю ширину (2 колонки)
    {
        MathButton *btn = makeCalcBtn("↵", true);
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            insertToTarget(btn->insertText(), btn->cursorOffset());
            QTimer::singleShot(0, this, [this]() {
                if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
            });
        });
        rightGrid->addWidget(btn, 2, 0, 1, 2);
    }

    // колонки и строки — одинаково растягиваются
    rightGrid->setColumnStretch(0, 1);
    rightGrid->setColumnStretch(1, 1);
    rightGrid->setRowStretch(0, 1);
    rightGrid->setRowStretch(1, 1);
    rightGrid->setRowStretch(2, 1);

    // ---------- собираем три блока ----------
    centerLay->addWidget(leftBlock,   4);
    centerLay->addWidget(centerBlock, 4);
    centerLay->addWidget(rightBlock,  2);

    // ---------- горизонтальное центрирование + растягивание до 1200 ----------
    QHBoxLayout *hosterLay = new QHBoxLayout();
    hosterLay->setContentsMargins(0, 0, 0, 0);
    hosterLay->setSpacing(0);

    hosterLay->addStretch(1);
    hosterLay->addWidget(centerHost, 1000000);
    hosterLay->addStretch(1);

    outer->addLayout(hosterLay);
    outer->addStretch(1); // пустота снизу

    return section;
}

QString MathPanel::keyboardLetterAt(int row, int col) const
{
    // Латинская раскладка (qwerty)
    static const QStringList latinLow = {
        "qwertyuiop",
        "asdfghjkl",
        "zxcvbnm"
    };
    static const QStringList latinUp = {
        "QWERTYUIOP",
        "ASDFGHJKL",
        "ZXCVBNM"
    };
    // Греческая раскладка (порядок как у Desmos)
    static const QStringList greekLow = {
        "αβγδεζηθικ",
        "λμνξοπρστυ",
        "φχψως"
    };
    static const QStringList greekUp = {
        "ΑΒΓΔΕΖΗΘΙΚ",
        "ΛΜΝΞΟΠΡΣΤΥ",
        "ΦΧΨΩΣ"
    };

    const bool up = m_keyboardShift;
    const QStringList &src =
        (m_keyboardLang == KeyboardLang::Latin)
            ? (up ? latinUp : latinLow)
            : (up ? greekUp : greekLow);

    if (row < 0 || row >= src.size()) return QString();
    const QString &line = src[row];
    if (col < 0 || col >= line.size()) return QString();
    return QString(line[col]);
}

void MathPanel::rebuildKeyboardContent(bool bigFont)
{
    if (!m_keyboardContent) return;

    // 1. Удаляем всех прямых детей m_keyboardContent
    const auto children = m_keyboardContent->findChildren<QWidget*>(
        QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *w : children) {
        w->setParent(nullptr);
        w->deleteLater();
    }

    // 2. Если у m_keyboardContent есть layout — переносим его на мусорку
    if (QLayout *old = m_keyboardContent->layout()) {
        if (!m_layoutTrash) {
            m_layoutTrash = new QWidget(this);
            m_layoutTrash->hide();
        }

        // если у мусорки уже что-то лежит — снести
        if (QLayout *prev = m_layoutTrash->layout()) {
            QLayoutItem *it;
            while ((it = prev->takeAt(0)) != nullptr) delete it;
            delete prev;
        }

        // переносим старый layout m_keyboardContent на мусорку
        m_layoutTrash->setLayout(old);
    }

    // --- дальше строим заново ---
    QVBoxLayout *outer = new QVBoxLayout(m_keyboardContent);
    outer->setContentsMargins(8, 6, 8, 6);
    outer->setSpacing(4);

    // три ряда букв: 10, 10, 7 + крайние Shift/Backspace в 3-м ряду
    // Итого 3-й ряд: [Shift] [z x c v b n m] [Backspace] = 9 кнопок
    const int rowSizes[2] = {10, 10};
    for (int r = 0; r < 2; ++r) {
        QHBoxLayout *hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(4);

        for (int c = 0; c < rowSizes[r]; ++c) {
            QString letter = keyboardLetterAt(r, c);
            if (letter.isEmpty()) continue;
            MathButton *btn = new MathButton(letter, letter);
            btn->setMinimumWidth(0);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                insertToTarget(btn->insertText(), btn->cursorOffset());
                QTimer::singleShot(0, this, [this]() {
                    if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
                });
            });
            hl->addWidget(btn, 1);
        }
        outer->addLayout(hl);
    }

    // --- третий ряд: Shift + 7 букв + Backspace ---
    {
        QHBoxLayout *hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(4);

        // Shift
        {
            MathButton *btn = new MathButton("⇧", "");
            if (m_keyboardShift) btn->setAccent(true);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, [this]() {
                toggleKeyboardShift();
            });
            hl->addWidget(btn, 1);
        }

        // 7 букв (третий ряд букв)
        for (int c = 0; c < 7; ++c) {
            QString letter = keyboardLetterAt(2, c);
            if (letter.isEmpty()) continue;
            MathButton *btn = new MathButton(letter, letter);
            btn->setMinimumWidth(0);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                insertToTarget(btn->insertText(), btn->cursorOffset());
                QTimer::singleShot(0, this, [this]() {
                    if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
                });
            });
            hl->addWidget(btn, 1);
        }

        // Backspace
        {
            MathButton *btn = new MathButton("⌫", "");
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, [this]() {
                if (!m_targetEdit) return;
                const QString t = m_targetEdit->text();
                const int pos = m_targetEdit->cursorPosition();
                if (pos > 0) {
                    m_targetEdit->setText(t.left(pos - 1) + t.mid(pos));
                    m_targetEdit->setCursorPosition(pos - 1);
                }
            });
            hl->addWidget(btn, 1);
        }

        outer->addLayout(hl);
    }

    // --- нижний ряд ---
    QHBoxLayout *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 0, 0, 0);
    bottom->setSpacing(4);

    // Lang
    {
        const QString langLabel = (m_keyboardLang == KeyboardLang::Latin) ? "αβγ" : "abc";
        MathButton *btn = new MathButton(langLabel, "");
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(btn, &QPushButton::clicked, this, [this]() {
            toggleKeyboardLang();
        });
        bottom->addWidget(btn, 2);
    }

    // спец-кнопки
    struct Key { QString label; QString insert; int cursor; int stretch; };
    QList<Key> specials;
    if (m_keyboardShift) {
        specials = {
                    {"xʸ", "^",  -1, 1},
                    {"'",  "'",  -1, 1},
                    {"]",  "]",  -1, 1},
                    {"}",  "}",  -1, 1},
                    {"%",  "%",  -1, 1},
                    {";",  ";",  -1, 1},
                    };
    } else {
        specials = {
                    {"a_b", "_",  -1, 1},
                    {"!",  "!",  -1, 1},
                    {"[]", "[]",  1, 1},
                    {"{}", "{}",  1, 1},
                    {"~",  "~",  -1, 1},
                    {":",  ":",  -1, 1},
                    };
    }
    for (const Key &k : specials) {
        MathButton *btn = new MathButton(k.label, k.insert);
        if (k.cursor >= 0) {
            connect(btn, &QPushButton::clicked, this, [this, k]() {
                if (!m_targetEdit) return;
                const QString t = m_targetEdit->text();
                const int pos = m_targetEdit->cursorPosition();
                m_targetEdit->setText(t.left(pos) + k.insert + t.mid(pos));
                m_targetEdit->setCursorPosition(pos + k.cursor);
                m_targetEdit->setFocus();
            });
        } else {
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                insertToTarget(btn->insertText(), btn->cursorOffset());
                QTimer::singleShot(0, this, [this]() {
                    if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
                });
            });
        }
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        bottom->addWidget(btn, k.stretch);
    }

    // Enter
    {
        MathButton *btn = new MathButton("↵", "\n", true);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(btn, &QPushButton::clicked, this, [this]() {
            insertToTarget("\n", -1);
            QTimer::singleShot(0, this, [this]() {
                if (m_lastFocusedWidget) m_lastFocusedWidget->setFocus(Qt::OtherFocusReason);
            });
        });
        bottom->addWidget(btn, 3);
    }

    outer->addLayout(bottom);
}

void MathPanel::toggleKeyboardLang()
{
    m_keyboardLang = (m_keyboardLang == KeyboardLang::Latin)
                         ? KeyboardLang::Greek
                         : KeyboardLang::Latin;
    rebuildKeyboardContent(width() > 700);
}

void MathPanel::toggleKeyboardShift()
{
    m_keyboardShift = !m_keyboardShift;
    rebuildKeyboardContent(width() > 700);
}

// ============ Раздел: Клавиатура ============
QWidget* MathPanel::createKeyboardSection()
{
    using namespace MathPalette;

    QWidget *section = new QWidget();
    section->setStyleSheet(QString("background-color: %1;").arg(BgRoot));

    QVBoxLayout *outer = new QVBoxLayout(section);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Контейнер с максимальной шириной и центрированием
    m_keyboardContent = new QWidget();
    m_keyboardContent->setMaximumWidth(900);
    m_keyboardContent->setMinimumWidth(0);
    m_keyboardContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *hosterLay = new QHBoxLayout();
    hosterLay->setContentsMargins(0, 0, 0, 0);
    hosterLay->setSpacing(0);
    hosterLay->addStretch(1);
    hosterLay->addWidget(m_keyboardContent, 1000000);
    hosterLay->addStretch(1);

    outer->addStretch(1);
    outer->addLayout(hosterLay);
    outer->addStretch(1);

    // Первая сборка (маленький шрифт)
    rebuildKeyboardContent(false);

    return section;
}

// ============ Вставка ============
void MathPanel::insertToTarget(const QString &text, int cursorOffset)
{
    if (!m_targetEdit) {
        emit functionInserted(text);
        return;
    }

    QString current = m_targetEdit->text();
    int pos = m_targetEdit->cursorPosition();
    QString newText = current.left(pos) + text + current.mid(pos);
    m_targetEdit->setText(newText);

    int newCursorPos = (cursorOffset >= 0)
                           ? pos + cursorOffset
                           : pos + text.length();
    m_targetEdit->setCursorPosition(newCursorPos);
    m_targetEdit->setFocus();

    emit functionInserted(text);
}

// ============ Слоты ============
void MathPanel::onMainSectionChanged(int index)
{
    if (!m_mainStack || index < 0 || index >= m_mainStack->count()) return;

    m_fadeAnim->stop();
    m_fadeAnim->setStartValue(0.0);
    m_fadeAnim->setEndValue(1.0);

    m_mainStack->setCurrentIndex(index);
    m_fadeAnim->start();
}

void MathPanel::onCategoryChanged(int index)
{
    if (!m_categoryStack || index < 0 || index >= m_categoryStack->count()) return;
    m_categoryStack->setCurrentIndex(index);
}
