#include "menupanel.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFont>
#include <QDebug>

// ==================================================================
// Тёмно-бежевая гамма
// ==================================================================
namespace MenuPalette {
const QString BgDeep        = "#1c1814";
const QString BgMain        = "#25201a";
const QString BgCard        = "#2f2921";
const QString BgCardHover   = "#3a332a";
const QString BgInset       = "#1a1612";
const QString BgSidebar     = "#1f1a15";

const QString AccentStart   = "#c9a876";
const QString AccentEnd     = "#e0c191";
const QString AccentHover   = "#d4b585";
const QString AccentPressed = "#a88a5c";
const QString AccentText    = "#e0c191";

const QString TextPrimary   = "#f0e6d6";
const QString TextSecondary = "#b8a890";
const QString TextMuted     = "#7a6e5e";
const QString TextOnAccent  = "#1c1814";

const QString Border        = "rgba(201,168,118,0.12)";
const QString BorderHover   = "rgba(201,168,118,0.3)";
}

// ==================================================================
// ProjectListItem — без изменений
// ==================================================================
ProjectListItem::ProjectListItem(const QString &name, const QString &description,
                                 const QString &modified, const QString &thumbnailColor,
                                 bool starred, QWidget *parent)
    : QWidget(parent), m_starred(starred)
{
    setFixedHeight(84);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_StyledBackground);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(18);

    m_thumbnail = new QFrame();
    m_thumbnail->setFixedSize(56, 56);
    m_thumbnail->setStyleSheet(QString(
                                   "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                   "    stop:0 %1, stop:1 %2);"
                                   "border-radius: 12px;"
                                   ).arg(thumbnailColor, MenuPalette::BgInset));
    layout->addWidget(m_thumbnail);

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"Liberation Serif\";"
                                 "font-size: 16pt;"
                                 "font-weight: 600;"
                                 ).arg(MenuPalette::TextPrimary));
    infoLayout->addWidget(nameLabel);

    QLabel *descLabel = new QLabel(description);
    descLabel->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"DejaVu Sans\";"
                                 "font-size: 13pt;"
                                 ).arg(MenuPalette::TextSecondary));
    infoLayout->addWidget(descLabel);

    layout->addLayout(infoLayout, 1);

    QLabel *dateLabel = new QLabel(modified);
    dateLabel->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"DejaVu Sans\";"
                                 "font-size: 11pt;"
                                 "letter-spacing: 0.5px;"
                                 ).arg(MenuPalette::TextMuted));
    dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(dateLabel);

    m_starButton = new QPushButton(m_starred ? "★" : "☆");
    m_starButton->setFixedSize(36, 36);
    m_starButton->setCursor(Qt::PointingHandCursor);
    m_starButton->setFlat(true);

    auto starStyle = [](bool starred) {
        return QString(
                   "QPushButton {"
                   "    background: transparent;"
                   "    border: none;"
                   "    color: %1;"
                   "    font-family: \"DejaVu Sans\";"
                   "    font-size: 16pt;"
                   "}"
                   "QPushButton:hover { color: %2; }"
                   ).arg(starred ? MenuPalette::AccentText : MenuPalette::TextMuted,
                 MenuPalette::AccentEnd);
    };
    m_starButton->setStyleSheet(starStyle(m_starred));
    connect(m_starButton, &QPushButton::clicked, this, [this, starStyle]() {
        m_starred = !m_starred;
        m_starButton->setText(m_starred ? "★" : "☆");
        m_starButton->setStyleSheet(starStyle(m_starred));
        emit starToggled(m_starred);
    });
    layout->addWidget(m_starButton);

    QLabel *arrow = new QLabel("→");
    arrow->setStyleSheet(QString(
                             "color: %1;"
                             "background: transparent;"
                             "font-family: \"DejaVu Sans\";"
                             "font-size: 16pt;"
                             "font-weight: bold;"
                             ).arg(MenuPalette::TextMuted));
    arrow->setFixedWidth(28);
    arrow->setAlignment(Qt::AlignCenter);
    layout->addWidget(arrow);
}

void ProjectListItem::mouseDoubleClickEvent(QMouseEvent *) { emit openRequested(); }

void ProjectListItem::enterEvent(QEnterEvent *)
{
    m_hovered = true;
    setStyleSheet(QString("QWidget { background: %1; border-radius: 12px; }")
                      .arg(MenuPalette::BgCardHover));
}

void ProjectListItem::leaveEvent(QEvent *)
{
    m_hovered = false;
    setStyleSheet("background: transparent;");
}

// ==================================================================
// SegmentedTab — без изменений
// ==================================================================
SegmentedTab::SegmentedTab(const QStringList &items, QWidget *parent)
    : QWidget(parent), m_items(items)
{
    setFixedHeight(42);
    setMinimumWidth(300);
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
}

void SegmentedTab::recalcRects()
{
    m_itemRects.clear();
    int n = m_items.size();
    if (n == 0) return;
    int w = width() / n;
    for (int i = 0; i < n; ++i) {
        int x = i * w;
        int width_i = (i == n - 1) ? (this->width() - x) : w;
        m_itemRects.append(QRect(x, 0, width_i, height()));
    }
}

void SegmentedTab::paintEvent(QPaintEvent *)
{
    using namespace MenuPalette;
    if (m_itemRects.size() != m_items.size()) recalcRects();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath bgPath;
    bgPath.addRoundedRect(rect().adjusted(0, 0, -1, -1), 10, 10);
    p.fillPath(bgPath, QColor(BgInset));
    p.setPen(QPen(QColor(Border), 1));
    p.drawPath(bgPath);

    if (m_currentIndex >= 0 && m_currentIndex < m_itemRects.size()) {
        QRect activeRect = m_itemRects[m_currentIndex].adjusted(3, 3, -3, -3);
        QPainterPath activePath;
        activePath.addRoundedRect(activeRect, 7, 7);
        QLinearGradient g(activeRect.topLeft(), activeRect.bottomRight());
        g.setColorAt(0, QColor(AccentStart));
        g.setColorAt(1, QColor(AccentEnd));
        p.fillPath(activePath, g);
    }

    for (int i = 0; i < m_items.size(); ++i) {
        QColor textColor;
        if (i == m_currentIndex) textColor = QColor(TextOnAccent);
        else if (i == m_hoveredIndex) textColor = QColor(AccentText);
        else textColor = QColor(TextSecondary);

        QFont f("DejaVu Sans", 13, i == m_currentIndex ? QFont::DemiBold : QFont::Normal);
        p.setFont(f);
        p.setPen(textColor);
        p.drawText(m_itemRects[i], Qt::AlignCenter, m_items[i]);
    }
}

void SegmentedTab::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < m_itemRects.size(); ++i) {
        if (m_itemRects[i].contains(event->pos())) { setCurrentIndex(i); break; }
    }
}

void SegmentedTab::mouseMoveEvent(QMouseEvent *event)
{
    int newHover = -1;
    for (int i = 0; i < m_itemRects.size(); ++i) {
        if (m_itemRects[i].contains(event->pos())) { newHover = i; break; }
    }
    if (newHover != m_hoveredIndex) { m_hoveredIndex = newHover; update(); }
}

void SegmentedTab::leaveEvent(QEvent *)
{
    m_hoveredIndex = -1;
    update();
}

void SegmentedTab::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.size() || index == m_currentIndex) return;
    m_currentIndex = index;
    update();
    emit currentChanged(index);
}

// ==================================================================
// MenuPanel
// ==================================================================
MenuPanel::MenuPanel(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet(QString("background: %1;").arg(MenuPalette::BgMain));
    setupUI();
}

MenuPanel::~MenuPanel() = default;

void MenuPanel::setupUI()
{
    using namespace MenuPalette;

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    outer->addWidget(createHeader());

    QWidget *body = new QWidget();
    body->setStyleSheet("background: transparent;");
    QHBoxLayout *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    bodyLayout->addWidget(createSidebar());

    QWidget *rightSide = new QWidget();
    rightSide->setStyleSheet(QString("background: %1;").arg(BgMain));
    QVBoxLayout *rightLayout = new QVBoxLayout(rightSide);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Хедер страницы
    QWidget *pageHeader = new QWidget();
    pageHeader->setStyleSheet(QString(
                                  "background: rgba(255,255,255,0.02);"
                                  "border-bottom: 1px solid %1;"
                                  ).arg(Border));
    QVBoxLayout *pageHeaderLayout = new QVBoxLayout(pageHeader);
    pageHeaderLayout->setContentsMargins(48, 32, 48, 28);
    pageHeaderLayout->setSpacing(8);

    m_pageTitle = new QLabel("Создать проект");
    m_pageTitle->setStyleSheet(QString(
                                   "color: %1;"
                                   "background: transparent;"
                                   "font-family: \"Liberation Serif\";"
                                   "font-size: 34pt;"
                                   "font-weight: bold;"
                                   "letter-spacing: -0.5px;"
                                   ).arg(TextPrimary));
    pageHeaderLayout->addWidget(m_pageTitle);

    m_pageSubtitle = new QLabel("Начать новый проект с чистого листа");
    m_pageSubtitle->setStyleSheet(QString(
                                      "color: %1;"
                                      "background: transparent;"
                                      "font-family: \"DejaVu Sans\";"
                                      "font-size: 15pt;"
                                      ).arg(TextMuted));
    pageHeaderLayout->addWidget(m_pageSubtitle);

    rightLayout->addWidget(pageHeader);

    m_contentStack = new QStackedWidget();
    m_contentStack->setStyleSheet("background: transparent;");
    m_contentStack->addWidget(createProjectsPage());  // 0
    m_contentStack->addWidget(createAccountPage());   // 1
    m_contentStack->addWidget(createLearnPage());     // 2
    rightLayout->addWidget(m_contentStack, 1);

    bodyLayout->addWidget(rightSide, 1);

    outer->addWidget(body, 1);
    outer->addWidget(createFooter());

    // ===== Начальное состояние: активна группа «Действия», пункт «Создать проект» =====
    m_updatingSelection = true;
    m_actionList->setCurrentRow(1);        // row 0 — заголовок группы
    m_accountList->setCurrentRow(-1);
    m_moreList->setCurrentRow(-1);
    m_updatingSelection = false;

    selectGroup(m_actionList, 1);
}

// ============ Хедер ============
QWidget* MenuPanel::createHeader()
{
    using namespace MenuPalette;

    QWidget *header = new QWidget();
    header->setStyleSheet(QString(
                              "background: %1;"
                              "border-bottom: 1px solid %2;"
                              ).arg(BgDeep, Border));
    header->setFixedHeight(110);

    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(48, 0, 48, 0);
    layout->setSpacing(24);

    QLabel *logo = new QLabel();
    logo->setFixedSize(60, 60);
    QPixmap logoPixmap(":/images/Riemann-multi.ico");
    if (!logoPixmap.isNull()) {
        logo->setPixmap(logoPixmap.scaled(60, 60, Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));
    } else {
        logo->setText("R");
        logo->setAlignment(Qt::AlignCenter);
        logo->setStyleSheet(QString(
                                "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                "    stop:0 %1, stop:1 %2);"
                                "color: %3;"
                                "border-radius: 15px;"
                                "font-family: \"Liberation Serif\";"
                                "font-size: 32pt;"
                                "font-weight: bold;"
                                ).arg(AccentStart, AccentEnd, TextOnAccent));
    }
    layout->addWidget(logo);

    QLabel *welcome = new QLabel("Добро пожаловать в Стереометрикс");
    welcome->setStyleSheet(QString(
                               "color: %1;"
                               "background: transparent;"
                               "font-family: \"Liberation Serif\";"
                               "font-size: 26pt;"
                               "font-weight: bold;"
                               "letter-spacing: -0.3px;"
                               ).arg(TextPrimary));
    welcome->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(welcome, 1);

    QLabel *version = new QLabel("v1.0.0");
    version->setStyleSheet(QString(
                               "color: %1;"
                               "background: %2;"
                               "border: 1px solid %3;"
                               "border-radius: 10px;"
                               "padding: 6px 14px;"
                               "font-family: \"DejaVu Sans\";"
                               "font-size: 13pt;"
                               ).arg(TextSecondary, BgCard, Border));
    layout->addWidget(version);

    return header;
}

// ============ Сайдбар ============
QWidget* MenuPanel::createSidebar()
{
    using namespace MenuPalette;

    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet(QString(
                               "background: %1;"
                               "border-right: 1px solid %2;"
                               ).arg(BgSidebar, Border));

    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(16, 20, 16, 20);
    layout->setSpacing(0);

    QFont itemFont("DejaVu Sans", 12);
    QFont categoryFont("DejaVu Sans", 11, QFont::Bold);
    categoryFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);

    const QString listStyle = QString(
                                  "QListWidget {"
                                  "    background: transparent;"
                                  "    border: none;"
                                  "    outline: none;"
                                  "    color: %1;"
                                  "}"
                                  "QListWidget::item {"
                                  "    background: transparent;"
                                  "    border-radius: 8px;"
                                  "    padding: 10px 12px;"
                                  "    margin: 1px 0;"
                                  "    color: %1;"
                                  "    border: none;"
                                  "    min-height: 22px;"
                                  "}"
                                  "QListWidget::item:hover {"
                                  "    background: rgba(201,168,118,0.08);"
                                  "    color: %2;"
                                  "}"
                                  "QListWidget::item:selected {"
                                  "    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                                  "        stop:0 %3,stop:1 %4);"
                                  "    color: %5;"
                                  "    font-weight: 600;"
                                  "}"
                                  ).arg(TextSecondary, TextPrimary,
                                       AccentStart, AccentEnd, TextOnAccent);

    auto makeList = [&]() {
        QListWidget *list = new QListWidget();
        list->setFocusPolicy(Qt::NoFocus);
        list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list->setSelectionMode(QAbstractItemView::SingleSelection);
        list->setStyleSheet(listStyle);
        list->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        return list;
    };

    auto addGroupHeader = [&categoryFont](QListWidget *list, const QString &text) {
        QListWidgetItem *item = new QListWidgetItem(text.toUpper());
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(0, 46));
        item->setFont(categoryFont);
        item->setForeground(QColor(MenuPalette::TextMuted));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        list->addItem(item);
    };

    auto addItem = [&itemFont](QListWidget *list, const QString &icon, const QString &text) {
        QListWidgetItem *item = new QListWidgetItem(QString("%1   %2").arg(icon, text));
        item->setSizeHint(QSize(0, 44));
        item->setFont(itemFont);
        list->addItem(item);
        return item;
    };

    auto fitHeight = [](QListWidget *list) {
        int h = 0;
        for (int i = 0; i < list->count(); ++i)
            h += list->item(i)->sizeHint().height();
        list->setFixedHeight(h + 2 * list->frameWidth());
    };

    // ===== Группа: Действия =====
    m_actionList = makeList();
    addGroupHeader(m_actionList, "Действия");
    addItem(m_actionList, "✦", "Создать проект");
    addItem(m_actionList, "◈", "Открыть проект");
    addItem(m_actionList, "▣", "Сохранить проект");
    addItem(m_actionList, "⇪", "Импортировать");
    addItem(m_actionList, "⇩", "Экспортировать");
    fitHeight(m_actionList);
    layout->addWidget(m_actionList);

    // ===== Растяжка =====
    layout->addStretch(1);

    // ===== Группа: Учётная запись =====
    m_accountList = makeList();
    addGroupHeader(m_accountList, "Учётная запись");
    addItem(m_accountList, "◉", "Профиль");
    fitHeight(m_accountList);
    layout->addWidget(m_accountList);

    // ===== Группа: Изучить больше =====
    m_moreList = makeList();
    addGroupHeader(m_moreList, "Изучить больше");
    addItem(m_moreList, "?", "Справка");
    addItem(m_moreList, "i", "О программе");
    addItem(m_moreList, "↗", "Онлайн-ресурсы");
    addItem(m_moreList, "§", "Документация");
    fitHeight(m_moreList);
    layout->addWidget(m_moreList);

    // ===== Сигналы =====
    // itemClicked срабатывает ВСЕГДА, даже при повторном клике по выбранному.
    connect(m_actionList,  &QListWidget::itemClicked,
            this, &MenuPanel::onActionClicked);
    connect(m_accountList, &QListWidget::itemClicked,
            this, &MenuPanel::onAccountClicked);
    connect(m_moreList,    &QListWidget::itemClicked,
            this, &MenuPanel::onMoreClicked);

    return sidebar;
}

// ============ Страница: Проекты ============
QWidget* MenuPanel::createProjectsPage()
{
    using namespace MenuPalette;

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scroll->setStyleSheet(QString(
                              "QScrollArea { border: none; background: transparent; }"
                              "QScrollBar:vertical { background: transparent; width: 6px; margin: 4px 0; }"
                              "QScrollBar::handle:vertical { background: %1; border-radius: 3px; min-height: 30px; }"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
                              ).arg(BorderHover));

    QWidget *content = new QWidget();
    content->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(48, 28, 48, 32);
    layout->setSpacing(28);

    QHBoxLayout *actionsLayout = new QHBoxLayout();
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(12);

    auto makeActionButton = [](const QString &icon, const QString &text, bool primary) {
        QPushButton *btn = new QPushButton(QString("%1  %2").arg(icon, text));
        btn->setFixedHeight(52);
        btn->setMinimumWidth(170);
        btn->setCursor(Qt::PointingHandCursor);
        if (primary) {
            btn->setStyleSheet(QString(
                                   "QPushButton {"
                                   "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                   "        stop:0 %1,stop:1 %2);"
                                   "    border: none;"
                                   "    border-radius: 10px;"
                                   "    color: %3;"
                                   "    font-family: \"DejaVu Sans\";"
                                   "    font-size: 14pt;"
                                   "    font-weight: 600;"
                                   "    padding: 0 22px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                   "        stop:0 %2,stop:1 %4);"
                                   "}"
                                   "QPushButton:pressed { background: %5; }"
                                   ).arg(AccentStart, AccentEnd, TextOnAccent,
                                        AccentHover, AccentPressed));
        } else {
            btn->setStyleSheet(QString(
                                   "QPushButton {"
                                   "    background: %1;"
                                   "    border: 1px solid %2;"
                                   "    border-radius: 10px;"
                                   "    color: %3;"
                                   "    font-family: \"DejaVu Sans\";"
                                   "    font-size: 14pt;"
                                   "    font-weight: 500;"
                                   "    padding: 0 22px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background: %4;"
                                   "    border-color: %5;"
                                   "    color: %6;"
                                   "}"
                                   ).arg(BgCard, Border, TextSecondary,
                                        BgCardHover, BorderHover, TextPrimary));
        }
        return btn;
    };

    m_createBtn = makeActionButton("✦", "Создать проект", true);
    connect(m_createBtn, &QPushButton::clicked, this, [this]() { emit createProjectRequested(); });
    actionsLayout->addWidget(m_createBtn);

    m_openBtn = makeActionButton("◈", "Открыть", false);
    connect(m_openBtn, &QPushButton::clicked, this, [this]() { emit openProjectRequested(); });
    actionsLayout->addWidget(m_openBtn);

    m_saveBtn = makeActionButton("▣", "Сохранить", false);
    connect(m_saveBtn, &QPushButton::clicked, this, [this]() { emit saveProjectRequested(); });
    actionsLayout->addWidget(m_saveBtn);

    m_importBtn = makeActionButton("⇪", "Импорт", false);
    connect(m_importBtn, &QPushButton::clicked, this, [this]() { emit importProjectRequested(); });
    actionsLayout->addWidget(m_importBtn);

    m_exportBtn = makeActionButton("⇩", "Экспорт", false);
    connect(m_exportBtn, &QPushButton::clicked, this, [this]() { emit exportProjectRequested(); });
    actionsLayout->addWidget(m_exportBtn);

    actionsLayout->addStretch();
    layout->addLayout(actionsLayout);

    QHBoxLayout *sectionHeader = new QHBoxLayout();
    sectionHeader->setContentsMargins(0, 0, 0, 0);
    sectionHeader->setSpacing(16);

    QLabel *sectionTitle = new QLabel("Ваши проекты");
    sectionTitle->setStyleSheet(QString(
                                    "color: %1;"
                                    "background: transparent;"
                                    "font-family: \"Liberation Serif\";"
                                    "font-size: 18pt;"
                                    "font-weight: 600;"
                                    ).arg(TextPrimary));
    sectionHeader->addWidget(sectionTitle);

    sectionHeader->addStretch();

    m_projectTabs = new SegmentedTab({ "Последние", "Избранные" });
    m_projectTabs->setFixedWidth(300);
    connect(m_projectTabs, &SegmentedTab::currentChanged,
            this, &MenuPanel::onProjectFilterChanged);
    sectionHeader->addWidget(m_projectTabs);

    layout->addLayout(sectionHeader);

    m_projectsStack = new QStackedWidget();
    m_projectsStack->setStyleSheet("background: transparent;");
    m_projectsStack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_projectsStack->addWidget(buildProjectList(false));
    m_projectsStack->addWidget(buildProjectList(true));
    m_projectsStack->setCurrentIndex(0);
    layout->addWidget(m_projectsStack);

    layout->addStretch();

    scroll->setWidget(content);
    return scroll;
}

// ============ Страница: Учётная запись ============
QWidget* MenuPanel::createAccountPage()
{
    using namespace MenuPalette;

    QWidget *page = new QWidget();
    page->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(48, 28, 48, 32);
    layout->setSpacing(24);

    QFrame *profileCard = new QFrame();
    profileCard->setStyleSheet(QString(
                                   "QFrame {"
                                   "    background: %1;"
                                   "    border: 1px solid %2;"
                                   "    border-radius: 16px;"
                                   "}"
                                   ).arg(BgCard, Border));
    QHBoxLayout *profileLayout = new QHBoxLayout(profileCard);
    profileLayout->setContentsMargins(28, 24, 28, 24);
    profileLayout->setSpacing(20);

    QLabel *avatar = new QLabel("Г");
    avatar->setFixedSize(72, 72);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
                              "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                              "    stop:0 %1, stop:1 %2);"
                              "color: %3;"
                              "border-radius: 36px;"
                              "font-family: \"Liberation Serif\";"
                              "font-size: 30pt;"
                              "font-weight: bold;"
                              ).arg(AccentStart, AccentEnd, TextOnAccent));
    profileLayout->addWidget(avatar);

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(6);

    QLabel *userName = new QLabel("Гость");
    userName->setStyleSheet(QString(
                                "color: %1;"
                                "background: transparent;"
                                "font-family: \"Liberation Serif\";"
                                "font-size: 22pt;"
                                "font-weight: bold;"
                                ).arg(TextPrimary));
    infoLayout->addWidget(userName);

    QLabel *userStatus = new QLabel("Локальный профиль · не выполнен вход");
    userStatus->setStyleSheet(QString(
                                  "color: %1;"
                                  "background: transparent;"
                                  "font-family: \"DejaVu Sans\";"
                                  "font-size: 15pt;"
                                  ).arg(TextMuted));
    infoLayout->addWidget(userStatus);

    profileLayout->addLayout(infoLayout, 1);

    QPushButton *signInBtn = new QPushButton("Войти в аккаунт");
    signInBtn->setFixedHeight(46);
    signInBtn->setMinimumWidth(180);
    signInBtn->setCursor(Qt::PointingHandCursor);
    signInBtn->setStyleSheet(QString(
                                 "QPushButton {"
                                 "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                 "        stop:0 %1,stop:1 %2);"
                                 "    border: none;"
                                 "    border-radius: 10px;"
                                 "    color: %3;"
                                 "    font-family: \"DejaVu Sans\";"
                                 "    font-size: 14pt;"
                                 "    font-weight: 600;"
                                 "    padding: 0 22px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                 "        stop:0 %2,stop:1 %4);"
                                 "}"
                                 ).arg(AccentStart, AccentEnd, TextOnAccent, AccentHover));
    connect(signInBtn, &QPushButton::clicked, this, [this]() { emit accountRequested(); });
    profileLayout->addWidget(signInBtn);

    layout->addWidget(profileCard);

    auto makeFeature = [](const QString &icon, const QString &title, const QString &desc) {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString(
                                "QFrame {"
                                "    background: %1;"
                                "    border: 1px solid %2;"
                                "    border-radius: 14px;"
                                "}"
                                ).arg(MenuPalette::BgCard, MenuPalette::Border));
        QVBoxLayout *l = new QVBoxLayout(card);
        l->setContentsMargins(24, 22, 24, 22);
        l->setSpacing(10);

        QLabel *ic = new QLabel(icon);
        ic->setStyleSheet(QString(
                              "color: %1;"
                              "background: transparent;"
                              "font-family: \"DejaVu Sans\";"
                              "font-size: 26pt;"
                              ).arg(MenuPalette::AccentText));
        l->addWidget(ic);

        QLabel *t = new QLabel(title);
        t->setStyleSheet(QString(
                             "color: %1;"
                             "background: transparent;"
                             "font-family: \"Liberation Serif\";"
                             "font-size: 16pt;"
                             "font-weight: 600;"
                             ).arg(MenuPalette::TextPrimary));
        l->addWidget(t);

        QLabel *d = new QLabel(desc);
        d->setStyleSheet(QString(
                             "color: %1;"
                             "background: transparent;"
                             "font-family: \"DejaVu Sans\";"
                             "font-size: 13pt;"
                             ).arg(MenuPalette::TextSecondary));
        d->setWordWrap(true);
        l->addWidget(d);

        return card;
    };

    QHBoxLayout *featuresLayout = new QHBoxLayout();
    featuresLayout->setContentsMargins(0, 0, 0, 0);
    featuresLayout->setSpacing(16);
    featuresLayout->addWidget(makeFeature("☁", "Облачная синхронизация",
                                          "Сохраняйте проекты в облаке и открывайте их на любом устройстве."));
    featuresLayout->addWidget(makeFeature("◈", "Общая библиотека",
                                          "Делитесь проектами и шаблонами с другими пользователями."));
    featuresLayout->addWidget(makeFeature("★", "Премиум-функции",
                                          "Расширенные инструменты визуализации и экспорт высокого качества."));
    layout->addLayout(featuresLayout);

    layout->addStretch();
    return page;
}

// ============ Страница: Изучить больше ============
QWidget* MenuPanel::createLearnPage()
{
    using namespace MenuPalette;

    QWidget *page = new QWidget();
    page->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(48, 28, 48, 32);
    layout->setSpacing(16);

    auto makeLink = [](const QString &icon, const QString &title,
                       const QString &desc) {
        QPushButton *card = new QPushButton();
        card->setCursor(Qt::PointingHandCursor);
        card->setFixedHeight(96);
        card->setStyleSheet(QString(
                                "QPushButton {"
                                "    background: %1;"
                                "    border: 1px solid %2;"
                                "    border-radius: 14px;"
                                "    text-align: left;"
                                "    padding: 0 28px;"
                                "}"
                                "QPushButton:hover {"
                                "    background: %3;"
                                "    border-color: %4;"
                                "}"
                                ).arg(BgCard, Border, BgCardHover, BorderHover));

        QHBoxLayout *l = new QHBoxLayout(card);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(20);

        QLabel *ic = new QLabel(icon);
        ic->setFixedSize(52, 52);
        ic->setAlignment(Qt::AlignCenter);
        ic->setStyleSheet(QString(
                              "background: rgba(201,168,118,0.1);"
                              "border-radius: 14px;"
                              "color: %1;"
                              "font-family: \"DejaVu Sans\";"
                              "font-size: 22pt;"
                              ).arg(AccentText));
        l->addWidget(ic);

        QVBoxLayout *textLayout = new QVBoxLayout();
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(4);

        QLabel *t = new QLabel(title);
        t->setStyleSheet(QString(
                             "color: %1;"
                             "background: transparent;"
                             "font-family: \"Liberation Serif\";"
                             "font-size: 16pt;"
                             "font-weight: 600;"
                             ).arg(TextPrimary));
        textLayout->addWidget(t);

        QLabel *d = new QLabel(desc);
        d->setStyleSheet(QString(
                             "color: %1;"
                             "background: transparent;"
                             "font-family: \"DejaVu Sans\";"
                             "font-size: 15pt;"
                             ).arg(TextSecondary));
        textLayout->addWidget(d);

        l->addLayout(textLayout, 1);

        QLabel *arrow = new QLabel("→");
        arrow->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"DejaVu Sans\";"
                                 "font-size: 18pt;"
                                 "font-weight: bold;"
                                 ).arg(AccentText));
        l->addWidget(arrow);

        return card;
    };

    QPushButton *docsCard = makeLink("§", "Документация",
                                     "Подробное руководство по работе с приложением");
    connect(docsCard, &QPushButton::clicked, this, [this]() { emit documentationRequested(); });
    layout->addWidget(docsCard);

    QPushButton *helpCard = makeLink("?", "Справка",
                                     "Быстрые ответы на частые вопросы");
    connect(helpCard, &QPushButton::clicked, this, [this]() { emit helpRequested(); });
    layout->addWidget(helpCard);

    QPushButton *onlineCard = makeLink("↗", "Онлайн-ресурсы",
                                       "Видеоуроки, статьи и сообщество");
    connect(onlineCard, &QPushButton::clicked, this, [this]() { emit onlineResourcesRequested(); });
    layout->addWidget(onlineCard);

    QPushButton *aboutCard = makeLink("i", "О программе",
                                      "Версия, лицензия, авторы");
    connect(aboutCard, &QPushButton::clicked, this, [this]() { emit aboutRequested(); });
    layout->addWidget(aboutCard);

    layout->addStretch();
    return page;
}

// ============ Список проектов ============
QWidget* MenuPanel::buildProjectList(bool onlyStarred)
{
    using namespace MenuPalette;

    QFrame *container = new QFrame();
    container->setStyleSheet(QString(
                                 "QFrame {"
                                 "    background: %1;"
                                 "    border: 1px solid %2;"
                                 "    border-radius: 16px;"
                                 "}"
                                 ).arg(BgCard, Border));

    QVBoxLayout *listLayout = new QVBoxLayout(container);
    listLayout->setContentsMargins(6, 6, 6, 6);
    listLayout->setSpacing(2);

    struct ProjectData {
        QString name;
        QString desc;
        QString modified;
        QString color;
        bool starred;
    };
    QList<ProjectData> allProjects = {
        { "Ознакомление со сферой Римана",
         "Стереографическая проекция сферы на плоскость",
         "2 часа назад", "#7b6bef", true },
        { "Создание проекций фигур",
         "Проецирование многогранников и кривых на плоскость",
         "Вчера", "#c9a876", true },
        { "Исследование спутниковой связи и времени её доступности в регионе",
         "Стереографическая проекция орбит и зон покрытия",
         "3 дня назад", "#14a3a8", false }
    };

    bool added = false;
    for (const auto &p : allProjects) {
        if (onlyStarred && !p.starred) continue;
        ProjectListItem *item = new ProjectListItem(p.name, p.desc, p.modified, p.color, p.starred);
        connect(item, &ProjectListItem::openRequested, this, [this, p]() {
            emit recentProjectRequested(p.name);
        });
        connect(item, &ProjectListItem::starToggled, this, [p](bool starred) {
            qDebug() << "Проект" << p.name << "избранное:" << starred;
        });
        listLayout->addWidget(item);
        added = true;
    }

    if (!added) {
        QLabel *empty = new QLabel(
            "Здесь пока пусто.\n"
            "Отмечайте проекты звёздочкой ★, чтобы они появились тут."
            );
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"DejaVu Sans\";"
                                 "font-size: 15pt;"
                                 "padding: 40px 20px;"
                                 ).arg(TextMuted));
        listLayout->addWidget(empty);
    }

    return container;
}

// ============ Футер ============
QWidget* MenuPanel::createFooter()
{
    using namespace MenuPalette;

    QWidget *footer = new QWidget();
    footer->setFixedHeight(56);
    footer->setStyleSheet(QString(
                              "background: %1;"
                              "border-top: 1px solid %2;"
                              ).arg(BgDeep, Border));

    QHBoxLayout *layout = new QHBoxLayout(footer);
    layout->setContentsMargins(48, 0, 48, 0);
    layout->setSpacing(28);

    auto makeLink = [](const QString &text) {
        QPushButton *btn = new QPushButton(text);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);
        btn->setStyleSheet(QString(
                               "QPushButton {"
                               "    background: transparent;"
                               "    border: none;"
                               "    color: %1;"
                               "    padding: 4px 0;"
                               "    font-family: \"DejaVu Sans\";"
                               "    font-size: 13pt;"
                               "}"
                               "QPushButton:hover { color: %2; }"
                               ).arg(MenuPalette::TextMuted, MenuPalette::AccentText));
        return btn;
    };

    layout->addWidget(makeLink("Документация"));
    layout->addWidget(makeLink("Сообщество"));
    layout->addStretch();

    QLabel *copyright = new QLabel("© 2025 Стереометрикс");
    copyright->setStyleSheet(QString(
                                 "color: %1;"
                                 "background: transparent;"
                                 "font-family: \"DejaVu Sans\";"
                                 "font-size: 11pt;"
                                 "letter-spacing: 0.5px;"
                                 ).arg(TextMuted));
    layout->addWidget(copyright);

    return footer;
}

// ==================================================================
// Централизованное переключение
// ==================================================================
void MenuPanel::selectGroup(QListWidget *active, int row)
{
    if (m_updatingSelection) return;
    if (!active || row <= 0) return;   // row 0 — заголовок группы

    // Снять текущую строку с неактивных списков (именно -1, а не clearSelection)
    m_updatingSelection = true;
    for (QListWidget *list : { m_actionList, m_accountList, m_moreList }) {
        if (list != active && list->currentRow() != -1)
            list->setCurrentRow(-1);
    }
    m_updatingSelection = false;

    struct PageInfo { QString title; QString subtitle; int stackIndex; };

    if (active == m_actionList) {
        static const QMap<int, PageInfo> pages = {
                                                  { 1, { "Создать проект",   "Начать новый проект с чистого листа",        0 } },
                                                  { 2, { "Открыть проект",   "Загрузить существующий проект с диска",      0 } },
                                                  { 3, { "Сохранить проект", "Сохранить текущий проект",                   0 } },
                                                  { 4, { "Импортировать",    "Импортировать проект из внешнего источника", 0 } },
                                                  { 5, { "Экспортировать",   "Экспортировать проект в другой формат",      0 } },
                                                  };
        auto it = pages.find(row);
        if (it == pages.end()) return;
        m_pageTitle->setText(it->title);
        m_pageSubtitle->setText(it->subtitle);
        m_contentStack->setCurrentIndex(it->stackIndex);
    }
    else if (active == m_accountList) {
        if (row != 1) return;
        m_pageTitle->setText("Профиль");
        m_pageSubtitle->setText("Управление учётной записью");
        m_contentStack->setCurrentIndex(1);
    }
    else if (active == m_moreList) {
        static const QMap<int, PageInfo> pages = {
                                                  { 1, { "Справка",        "Ответы на частые вопросы",       2 } },
                                                  { 2, { "О программе",    "Информация о приложении",        2 } },
                                                  { 3, { "Онлайн-ресурсы", "Полезные ссылки и сообщество",   2 } },
                                                  { 4, { "Документация",   "Руководство пользователя",       2 } },
                                                  };
        auto it = pages.find(row);
        if (it == pages.end()) return;
        m_pageTitle->setText(it->title);
        m_pageSubtitle->setText(it->subtitle);
        m_contentStack->setCurrentIndex(it->stackIndex);
    }
}

// ============ Слоты ============
void MenuPanel::onActionClicked(QListWidgetItem *item)
{
    if (!item) return;
    // row 0 — заголовок группы, у него флаг NoItemFlags, но на всякий случай
    int row = m_actionList->row(item);
    if (row <= 0) return;

    // Явно выставляем текущую строку (важно, если кликнули по уже выбранной)
    if (m_actionList->currentRow() != row)
        m_actionList->setCurrentRow(row);

    selectGroup(m_actionList, row);
}

void MenuPanel::onAccountClicked(QListWidgetItem *item)
{
    if (!item) return;
    int row = m_accountList->row(item);
    if (row <= 0) return;

    if (m_accountList->currentRow() != row)
        m_accountList->setCurrentRow(row);

    selectGroup(m_accountList, row);
}

void MenuPanel::onMoreClicked(QListWidgetItem *item)
{
    if (!item) return;
    int row = m_moreList->row(item);
    if (row <= 0) return;

    if (m_moreList->currentRow() != row)
        m_moreList->setCurrentRow(row);

    selectGroup(m_moreList, row);
}

void MenuPanel::onProjectFilterChanged(int index)
{
    if (m_projectsStack) m_projectsStack->setCurrentIndex(index);
}
