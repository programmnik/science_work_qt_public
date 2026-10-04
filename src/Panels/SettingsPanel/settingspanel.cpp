#include "settingspanel.h"
#include <QFrame>
#include <QFont>
#include <QScrollArea>
#include <QDebug>

// ============ Цветовая гамма для настроек (фиолетово-синяя) ============
namespace SettingsPalette {
const QString AccentStart   = "#5b4bdb";
const QString AccentEnd     = "#7b6bef";
const QString AccentHover   = "#6b5be6";
const QString AccentPressed = "#4a3bc0";
const QString AccentText    = "#a89bff";

const QString BgCard        = "rgba(255,255,255,0.03)";
const QString BgCardBorder  = "rgba(255,255,255,0.06)";
const QString BgInput       = "rgba(255,255,255,0.06)";
const QString BgInputBorder = "rgba(255,255,255,0.1)";

const QString TextPrimary   = "#e8e8f0";
const QString TextSecondary = "#a0a0b8";
const QString TextMuted     = "#6a6a80";

const QString SidebarBg     = "#1e1b3a";
const QString SidebarItem   = "rgba(255,255,255,0.04)";
const QString SidebarHover  = "rgba(91,75,219,0.25)";
const QString SidebarActive = "qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #5b4bdb,stop:1 #7b6bef)";

// Шрифты — встроенные Qt-семейства
const QString Serif = "\"Liberation Serif\"";
const QString Sans  = "\"DejaVu Sans\"";
}

SettingsPanel::SettingsPanel(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent;");
    setupUI();
}

SettingsPanel::~SettingsPanel() = default;

void SettingsPanel::setupUI()
{
    using namespace SettingsPalette;

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ============ ЛЕВАЯ ПАНЕЛЬ ============
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(260);
    sidebar->setStyleSheet(QString(
                               "background: %1;"
                               "border-right: 1px solid rgba(255,255,255,0.05);"
                               ).arg(SidebarBg));

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 30);
    sidebarLayout->setSpacing(16);

    // Заголовок сайдбара
    QLabel *sidebarTitle = new QLabel("Настройки");
    sidebarTitle->setStyleSheet(QString(
                                    "color: %1;"
                                    "background: transparent;"
                                    "font-family: %2;"
                                    "font-size: 22px;"
                                    "font-weight: 600;"
                                    "letter-spacing: -0.3px;"
                                    ).arg(TextPrimary, Serif));
    sidebarLayout->addWidget(sidebarTitle);

    QLabel *sidebarSubtitle = new QLabel("Конфигурация приложения");
    sidebarSubtitle->setStyleSheet(QString(
                                       "color: %1;"
                                       "background: transparent;"
                                       "font-family: %2;"
                                       "font-size: 12px;"
                                       "letter-spacing: 0.2px;"
                                       ).arg(TextMuted, Sans));
    sidebarLayout->addWidget(sidebarSubtitle);

    sidebarLayout->addSpacing(20);

    // Список категорий
    m_categoryList = new QListWidget();
    m_categoryList->setFocusPolicy(Qt::NoFocus);
    m_categoryList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_categoryList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_categoryList->setStyleSheet(QString(
                                      "QListWidget {"
                                      "    background: transparent;"
                                      "    border: none;"
                                      "    outline: none;"
                                      "    color: %1;"
                                      "    font-family: %6;"
                                      "    font-size: 14px;"
                                      "}"
                                      "QListWidget::item {"
                                      "    background: %2;"
                                      "    border-radius: 10px;"
                                      "    padding: 12px 16px;"
                                      "    margin: 4px 0px;"
                                      "    color: %1;"
                                      "    border: none;"
                                      "}"
                                      "QListWidget::item:hover {"
                                      "    background: %3;"
                                      "    color: %4;"
                                      "}"
                                      "QListWidget::item:selected {"
                                      "    background: %5;"
                                      "    color: #ffffff;"
                                      "    font-weight: 600;"
                                      "}"
                                      ).arg(TextSecondary, SidebarItem, SidebarHover,
                                           TextPrimary, SidebarActive, Sans));

    struct Category { QString icon; QString name; };
    QList<Category> categories = {
        { "🎨", "Графика" },
        { "🖥", "Интерфейс" },
        { "⚡", "Производительность" },
        { "ℹ", "О программе" }
    };
    for (const auto &cat : categories) {
        QListWidgetItem *item = new QListWidgetItem(QString("%1  %2").arg(cat.icon, cat.name));
        item->setSizeHint(QSize(0, 44));
        m_categoryList->addItem(item);
    }
    m_categoryList->setCurrentRow(0);
    connect(m_categoryList, &QListWidget::currentRowChanged,
            this, &SettingsPanel::onCategoryChanged);

    sidebarLayout->addWidget(m_categoryList, 1);

    // Кнопки
    QPushButton *resetButton = new QPushButton("Сбросить");
    resetButton->setFixedHeight(40);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setStyleSheet(QString(
                                   "QPushButton {"
                                   "    background: transparent;"
                                   "    border: 1px solid rgba(255,255,255,0.1);"
                                   "    border-radius: 10px;"
                                   "    color: %1;"
                                   "    font-family: %4;"
                                   "    font-size: 13px;"
                                   "    font-weight: 600;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background: rgba(255,255,255,0.06);"
                                   "    border-color: %2;"
                                   "    color: %3;"
                                   "}"
                                   ).arg(TextSecondary, AccentStart, TextPrimary, Sans));
    connect(resetButton, &QPushButton::clicked, this, &SettingsPanel::onResetClicked);
    sidebarLayout->addWidget(resetButton);

    QPushButton *applyButton = new QPushButton("Применить");
    applyButton->setFixedHeight(44);
    applyButton->setCursor(Qt::PointingHandCursor);
    applyButton->setStyleSheet(QString(
                                   "QPushButton {"
                                   "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
                                   "    border: none;"
                                   "    border-radius: 10px;"
                                   "    color: #ffffff;"
                                   "    font-family: %5;"
                                   "    font-size: 14px;"
                                   "    font-weight: 600;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %2,stop:1 %3);"
                                   "}"
                                   "QPushButton:pressed {"
                                   "    background: %4;"
                                   "}"
                                   ).arg(AccentStart, AccentEnd, AccentHover,
                                        AccentPressed, Sans));
    connect(applyButton, &QPushButton::clicked, this, &SettingsPanel::onApplyClicked);
    sidebarLayout->addWidget(applyButton);

    mainLayout->addWidget(sidebar);

    // ============ ПРАВАЯ ЧАСТЬ ============
    QWidget *rightSide = new QWidget();
    rightSide->setStyleSheet("background: transparent;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightSide);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Хедер
    QWidget *headerWidget = new QWidget();
    headerWidget->setStyleSheet(QString(
                                    "background: rgba(255,255,255,0.02);"
                                    "border-bottom: 1px solid %1;"
                                    ).arg(BgCardBorder));
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(48, 30, 48, 24);
    headerLayout->setSpacing(6);

    m_pageTitle = new QLabel("Графика");
    m_pageTitle->setStyleSheet(QString(
                                   "color: %1;"
                                   "background: transparent;"
                                   "font-family: %2;"
                                   "font-size: 26px;"
                                   "font-weight: 600;"
                                   "letter-spacing: -0.3px;"
                                   ).arg(TextPrimary, Serif));
    headerLayout->addWidget(m_pageTitle);

    m_pageSubtitle = new QLabel("Настройки отображения и рендеринга сцены");
    m_pageSubtitle->setStyleSheet(QString(
                                      "color: %1;"
                                      "background: transparent;"
                                      "font-family: %2;"
                                      "font-size: 13px;"
                                      "letter-spacing: 0.2px;"
                                      ).arg(TextMuted, Sans));
    headerLayout->addWidget(m_pageSubtitle);

    rightLayout->addWidget(headerWidget);

    // Стек контента
    m_contentStack = new QStackedWidget();
    m_contentStack->setStyleSheet("background: transparent;");
    m_contentStack->addWidget(createGraphicsPage());
    m_contentStack->addWidget(createInterfacePage());
    m_contentStack->addWidget(createPerformancePage());
    m_contentStack->addWidget(createAboutPage());

    rightLayout->addWidget(m_contentStack, 1);

    mainLayout->addWidget(rightSide, 1);
}

// ============ Хелпер: страница со скроллом ============
static QScrollArea* makeScrollPage(QWidget *content)
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet(QString(
                              "QScrollArea { border: none; background: transparent; }"
                              "QScrollBar:vertical { background: rgba(255,255,255,0.03); width: 6px; border-radius: 3px; margin: 4px 0; }"
                              "QScrollBar::handle:vertical { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 %1,stop:1 %2); border-radius: 3px; min-height: 30px; }"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
                              ).arg(SettingsPalette::AccentStart, SettingsPalette::AccentEnd));
    scroll->setWidget(content);
    return scroll;
}

// ============ Хелпер: строка настройки ============
static QWidget* makeRow(const QString &label, QWidget *control, QWidget *valueLabel = nullptr)
{
    using namespace SettingsPalette;
    QWidget *row = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    QLabel *lbl = new QLabel(label);
    lbl->setStyleSheet(QString(
                           "color: %1;"
                           "background: transparent;"
                           "font-family: %2;"
                           "font-size: 14px;"
                           ).arg(TextSecondary, Sans));
    lbl->setMinimumWidth(180);
    layout->addWidget(lbl);

    layout->addWidget(control, 1);
    if (valueLabel) layout->addWidget(valueLabel);

    return row;
}

// ============ Стили контролов ============
static QString sliderStyle()
{
    using namespace SettingsPalette;
    return QString(
               "QSlider::groove:horizontal { height: 4px; background: rgba(255,255,255,0.1); border-radius: 2px; }"
               "QSlider::handle:horizontal { background: %1; width: 14px; height: 14px; margin: -5px 0; border-radius: 7px; }"
               "QSlider::handle:horizontal:hover { background: %2; width: 16px; height: 16px; margin: -6px 0; border-radius: 8px; }"
               "QSlider::sub-page:horizontal { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 %1,stop:1 %2); border-radius: 2px; }"
               ).arg(AccentStart, AccentEnd);
}

static QString comboStyle()
{
    using namespace SettingsPalette;
    return QString(
               "QComboBox {"
               "    background: %1;"
               "    border: 1px solid %2;"
               "    border-radius: 8px;"
               "    padding: 6px 12px;"
               "    color: %3;"
               "    font-family: %6;"
               "    font-size: 14px;"
               "    min-width: 150px;"
               "}"
               "QComboBox:hover { border-color: %4; }"
               "QComboBox::drop-down { border: none; width: 24px; }"
               "QComboBox::down-arrow {"
               "    image: none;"
               "    border-left: 4px solid transparent;"
               "    border-right: 4px solid transparent;"
               "    border-top: 5px solid %5;"
               "    margin-right: 8px;"
               "}"
               "QComboBox QAbstractItemView {"
               "    background: #1e1b3a;"
               "    border: 1px solid %2;"
               "    color: %3;"
               "    font-family: %6;"
               "    font-size: 14px;"
               "    selection-background-color: %4;"
               "    outline: none;"
               "}"
               ).arg(BgInput, BgInputBorder, TextPrimary, AccentStart, TextMuted, Sans);
}

static QString checkStyle()
{
    using namespace SettingsPalette;
    return QString(
               "QCheckBox {"
               "    color: %1;"
               "    background: transparent;"
               "    font-family: %4;"
               "    font-size: 14px;"
               "    spacing: 10px;"
               "}"
               "QCheckBox::indicator {"
               "    width: 20px; height: 20px;"
               "    border-radius: 5px;"
               "    border: 2px solid rgba(255,255,255,0.15);"
               "    background: rgba(255,255,255,0.03);"
               "}"
               "QCheckBox::indicator:hover { border-color: %2; }"
               "QCheckBox::indicator:checked {"
               "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %2,stop:1 %3);"
               "    border-color: %3;"
               "}"
               ).arg(TextSecondary, AccentStart, AccentEnd, Sans);
}

static QString spinStyle()
{
    using namespace SettingsPalette;
    return QString(
               "QSpinBox {"
               "    background: %1;"
               "    border: 1px solid %2;"
               "    border-radius: 8px;"
               "    padding: 6px 12px;"
               "    color: %3;"
               "    font-family: %5;"
               "    font-size: 14px;"
               "    min-width: 100px;"
               "}"
               "QSpinBox:hover { border-color: %4; }"
               "QSpinBox::up-button, QSpinBox::down-button {"
               "    background: rgba(255,255,255,0.06);"
               "    border: none;"
               "    width: 20px;"
               "}"
               "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
               "    background: %4;"
               "}"
               ).arg(BgInput, BgInputBorder, TextPrimary, AccentStart, Sans);
}

static QString valueLabelStyle()
{
    using namespace SettingsPalette;
    return QString(
               "color: %1;"
               "background: transparent;"
               "font-family: %2;"
               "font-size: 14px;"
               "font-weight: 600;"
               "min-width: 55px;"
               ).arg(AccentText, Sans);
}

static QString sectionTitleStyle()
{
    using namespace SettingsPalette;
    return QString(
               "color: %1;"
               "background: transparent;"
               "font-family: %2;"
               "font-size: 13px;"
               "font-weight: 600;"
               "letter-spacing: 0.8px;"
               "padding: 8px 0 4px 0;"
               ).arg(TextMuted, Sans);
}

// ============ Страница: Графика ============
QWidget* SettingsPanel::createGraphicsPage()
{
    QWidget *content = new QWidget();
    content->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(48, 32, 48, 32);
    layout->setSpacing(20);

    QLabel *sectionTitle = new QLabel("ОСНОВНЫЕ ПАРАМЕТРЫ");
    sectionTitle->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle);

    m_fovSlider = new QSlider(Qt::Horizontal);
    m_fovSlider->setRange(45, 120);
    m_fovSlider->setValue(75);
    m_fovSlider->setStyleSheet(sliderStyle());
    m_fovValue = new QLabel("75°");
    m_fovValue->setStyleSheet(valueLabelStyle());
    m_fovValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    connect(m_fovSlider, &QSlider::valueChanged, this, [this](int v) {
        m_fovValue->setText(QString::number(v) + "°");
    });
    layout->addWidget(makeRow("Угол обзора (FOV)", m_fovSlider, m_fovValue));

    m_renderDistanceSlider = new QSlider(Qt::Horizontal);
    m_renderDistanceSlider->setRange(50, 500);
    m_renderDistanceSlider->setValue(200);
    m_renderDistanceSlider->setStyleSheet(sliderStyle());
    m_renderDistanceValue = new QLabel("200");
    m_renderDistanceValue->setStyleSheet(valueLabelStyle());
    m_renderDistanceValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    connect(m_renderDistanceSlider, &QSlider::valueChanged, this, [this](int v) {
        m_renderDistanceValue->setText(QString::number(v));
    });
    layout->addWidget(makeRow("Дальность прорисовки", m_renderDistanceSlider, m_renderDistanceValue));

    m_qualityCombo = new QComboBox();
    m_qualityCombo->addItems({"Низкое", "Среднее", "Высокое", "Ультра"});
    m_qualityCombo->setCurrentIndex(2);
    m_qualityCombo->setStyleSheet(comboStyle());
    layout->addWidget(makeRow("Качество графики", m_qualityCombo));

    layout->addSpacing(12);

    QLabel *sectionTitle2 = new QLabel("ДОПОЛНИТЕЛЬНО");
    sectionTitle2->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle2);

    m_vsyncCheck = new QCheckBox("Вертикальная синхронизация (VSync)");
    m_vsyncCheck->setChecked(true);
    m_vsyncCheck->setStyleSheet(checkStyle());
    layout->addWidget(m_vsyncCheck);

    m_antialiasingCheck = new QCheckBox("Сглаживание (Anti-aliasing)");
    m_antialiasingCheck->setChecked(true);
    m_antialiasingCheck->setStyleSheet(checkStyle());
    layout->addWidget(m_antialiasingCheck);

    layout->addStretch();

    return makeScrollPage(content);
}

// ============ Страница: Интерфейс ============
QWidget* SettingsPanel::createInterfacePage()
{
    QWidget *content = new QWidget();
    content->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(48, 32, 48, 32);
    layout->setSpacing(20);

    QLabel *sectionTitle = new QLabel("ОФОРМЛЕНИЕ");
    sectionTitle->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle);

    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({"Тёмная", "Светлая", "Системная"});
    m_themeCombo->setCurrentIndex(0);
    m_themeCombo->setStyleSheet(comboStyle());
    layout->addWidget(makeRow("Тема оформления", m_themeCombo));

    m_uiScaleSlider = new QSlider(Qt::Horizontal);
    m_uiScaleSlider->setRange(75, 150);
    m_uiScaleSlider->setValue(100);
    m_uiScaleSlider->setStyleSheet(sliderStyle());
    m_uiScaleValue = new QLabel("100%");
    m_uiScaleValue->setStyleSheet(valueLabelStyle());
    m_uiScaleValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    connect(m_uiScaleSlider, &QSlider::valueChanged, this, [this](int v) {
        m_uiScaleValue->setText(QString::number(v) + "%");
    });
    layout->addWidget(makeRow("Масштаб интерфейса", m_uiScaleSlider, m_uiScaleValue));

    layout->addSpacing(12);

    QLabel *sectionTitle2 = new QLabel("ПОВЕДЕНИЕ");
    sectionTitle2->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle2);

    m_showHintsCheck = new QCheckBox("Показывать подсказки при наведении");
    m_showHintsCheck->setChecked(true);
    m_showHintsCheck->setStyleSheet(checkStyle());
    layout->addWidget(m_showHintsCheck);

    layout->addStretch();

    return makeScrollPage(content);
}

// ============ Страница: Производительность ============
QWidget* SettingsPanel::createPerformancePage()
{
    QWidget *content = new QWidget();
    content->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(48, 32, 48, 32);
    layout->setSpacing(20);

    QLabel *sectionTitle = new QLabel("РЕНДЕРИНГ");
    sectionTitle->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle);

    m_fpsLimitSpin = new QSpinBox();
    m_fpsLimitSpin->setRange(30, 360);
    m_fpsLimitSpin->setValue(60);
    m_fpsLimitSpin->setSuffix(" FPS");
    m_fpsLimitSpin->setStyleSheet(spinStyle());
    layout->addWidget(makeRow("Лимит FPS", m_fpsLimitSpin));

    layout->addSpacing(12);

    QLabel *sectionTitle2 = new QLabel("ОПТИМИЗАЦИЯ");
    sectionTitle2->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle2);

    m_multithreadingCheck = new QCheckBox("Многопоточная обработка");
    m_multithreadingCheck->setChecked(true);
    m_multithreadingCheck->setStyleSheet(checkStyle());
    layout->addWidget(m_multithreadingCheck);

    m_showFpsCheck = new QCheckBox("Отображать счётчик FPS");
    m_showFpsCheck->setChecked(false);
    m_showFpsCheck->setStyleSheet(checkStyle());
    layout->addWidget(m_showFpsCheck);

    layout->addStretch();

    return makeScrollPage(content);
}

// ============ Страница: О программе ============
QWidget* SettingsPanel::createAboutPage()
{
    using namespace SettingsPalette;

    QWidget *content = new QWidget();
    content->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(48, 32, 48, 32);
    layout->setSpacing(20);

    QLabel *sectionTitle = new QLabel("ИНФОРМАЦИЯ");
    sectionTitle->setStyleSheet(sectionTitleStyle());
    layout->addWidget(sectionTitle);

    auto makeInfoRow = [](const QString &label, const QString &value) {
        QWidget *row = new QWidget();
        QHBoxLayout *l = new QHBoxLayout(row);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(16);
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet(QString(
                               "color: %1;"
                               "background: transparent;"
                               "font-family: %2;"
                               "font-size: 14px;"
                               ).arg(TextSecondary, Sans));
        lbl->setMinimumWidth(180);
        l->addWidget(lbl);
        QLabel *val = new QLabel(value);
        val->setStyleSheet(QString(
                               "color: %1;"
                               "background: transparent;"
                               "font-family: %2;"
                               "font-size: 14px;"
                               "font-weight: 600;"
                               ).arg(TextPrimary, Sans));
        l->addWidget(val, 1);
        return row;
    };

    layout->addWidget(makeInfoRow("Версия приложения", "1.0.0"));
    layout->addWidget(makeInfoRow("Дата сборки", QString(__DATE__)));
    layout->addWidget(makeInfoRow("Qt версия", QString(qVersion())));
    layout->addWidget(makeInfoRow("Платформа", "Cross-platform"));

    layout->addStretch();

    return makeScrollPage(content);
}

// ============ Слоты ============
void SettingsPanel::onCategoryChanged(int index)
{
    if (index < 0 || index >= m_contentStack->count()) return;
    m_contentStack->setCurrentIndex(index);

    struct PageInfo { QString title; QString subtitle; };
    static const QList<PageInfo> pages = {
        { "Графика",             "Настройки отображения и рендеринга сцены" },
        { "Интерфейс",           "Внешний вид и поведение элементов управления" },
        { "Производительность",  "Оптимизация и параметры рендеринга" },
        { "О программе",         "Информация о приложении" }
    };
    if (index < pages.size()) {
        m_pageTitle->setText(pages[index].title);
        m_pageSubtitle->setText(pages[index].subtitle);
    }
}

void SettingsPanel::onResetClicked()
{
    m_fovSlider->setValue(75);
    m_renderDistanceSlider->setValue(200);
    m_qualityCombo->setCurrentIndex(2);
    m_vsyncCheck->setChecked(true);
    m_antialiasingCheck->setChecked(true);
    m_themeCombo->setCurrentIndex(0);
    m_uiScaleSlider->setValue(100);
    m_showHintsCheck->setChecked(true);
    m_fpsLimitSpin->setValue(60);
    m_multithreadingCheck->setChecked(true);
    m_showFpsCheck->setChecked(false);
    qDebug() << "Настройки сброшены";
}

void SettingsPanel::onApplyClicked()
{
    qDebug() << "Применены настройки:"
             << "FOV:" << m_fovSlider->value()
             << "Distance:" << m_renderDistanceSlider->value()
             << "Quality:" << m_qualityCombo->currentText()
             << "VSync:" << m_vsyncCheck->isChecked()
             << "AA:" << m_antialiasingCheck->isChecked()
             << "Theme:" << m_themeCombo->currentText()
             << "UIScale:" << m_uiScaleSlider->value()
             << "Hints:" << m_showHintsCheck->isChecked()
             << "FPS Limit:" << m_fpsLimitSpin->value()
             << "Multithreading:" << m_multithreadingCheck->isChecked()
             << "Show FPS:" << m_showFpsCheck->isChecked();

    emit settingsChanged();
}
