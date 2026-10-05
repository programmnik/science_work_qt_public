#include "customtitlebar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <QPainterPath>
#include <QWindow>
#include <QApplication>

// ===========================================================================
//  SVG-иконки по умолчанию
// ===========================================================================
namespace {

QString makeSvgLogo()
{
    return R"SVG(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none">
  <circle cx="12" cy="12" r="9" stroke="#00B4D8" stroke-width="1.8"/>
  <ellipse cx="12" cy="12" rx="9" ry="3.5" stroke="#00B4D8" stroke-dasharray="2 2"/>
  <path d="M12 3 L12 21" stroke="#60A5FA"/>
  <path d="M3 12 L21 12" stroke="#F87171"/>
  <circle cx="12" cy="8.5" r="2" fill="#FBBF24"/>
</svg>)SVG";
}

QString makeSvgMinimize(const QColor &c)
{
    return QStringLiteral(
               "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'>"
               "<line x1='3' y1='12' x2='13' y2='12' "
               "stroke='%1' stroke-width='1.4'/></svg>").arg(c.name());
}

QString makeSvgMaximize(const QColor &c)
{
    return QStringLiteral(
               "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='none'>"
               "<rect x='3.5' y='3.5' width='9' height='9' "
               "stroke='%1' stroke-width='1.4'/></svg>").arg(c.name());
}

QString makeSvgRestore(const QColor &c)
{
    return QStringLiteral(
               "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='none'>"
               "<rect x='3' y='5' width='8' height='8' stroke='%1' stroke-width='1.4'/>"
               "<path d='M5 5 V3 H13 V11 H11' stroke='%1' stroke-width='1.4' fill='none'/>"
               "</svg>").arg(c.name());
}

QString makeSvgClose(const QColor &c)
{
    return QStringLiteral(
               "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'>"
               "<line x1='4' y1='4' x2='12' y2='12' stroke='%1' stroke-width='1.4'/>"
               "<line x1='12' y1='4' x2='4' y2='12' stroke='%1' stroke-width='1.4'/>"
               "</svg>").arg(c.name());
}

QIcon renderSvgIcon(const QString &svg, int size)
{
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    renderer.render(&p, QRectF(0, 0, size, size));
    p.end();
    return QIcon(pm);
}

} // namespace

// ===========================================================================
//  Тема по умолчанию
// ===========================================================================
TitleBarTheme TitleBarTheme::defaultDark()
{
    TitleBarTheme t;
    t.svgLogo     = makeSvgLogo();
    t.svgMinimize = makeSvgMinimize(t.btnIcon);
    t.svgMaximize = makeSvgMaximize(t.btnIcon);
    t.svgRestore  = makeSvgRestore(t.btnIcon);
    t.svgClose    = makeSvgClose(t.btnIcon);
    return t;
}

// ===========================================================================
//  CustomTitleBar
// ===========================================================================
CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent)
    , m_theme(TitleBarTheme::defaultDark())
{
    setFixedHeight(m_theme.height);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground, false);

    buildUi();
    applyTheme();
}

void CustomTitleBar::buildUi()
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(8, 0, 8, 0);
    lay->setSpacing(8);

    m_logo = new QLabel(this);
    m_logo->setAttribute(Qt::WA_TranslucentBackground, true);
    m_logo->setAttribute(Qt::WA_NoSystemBackground, true);
    m_logo->setAutoFillBackground(false);
    m_logo->setStyleSheet("QLabel { background: transparent; border: none; }");
    m_logo->setFixedSize(20, 20);
    m_logo->setScaledContents(false);
    lay->addWidget(m_logo);

    // Левая группа: название + версия
    auto *titleBox = new QWidget(this);
    titleBox->setObjectName("titleBox");
    titleBox->setAttribute(Qt::WA_TranslucentBackground, true);
    titleBox->setAttribute(Qt::WA_NoSystemBackground, true);
    titleBox->setAutoFillBackground(false);
    titleBox->setStyleSheet("QWidget#titleBox { background: transparent; border: none; }");
    auto *tLay = new QHBoxLayout(titleBox);
    tLay->setContentsMargins(0, 0, 0, 0);
    tLay->setSpacing(6);

    m_appTitle = new QLabel(this);
    m_appSub   = new QLabel(this);
    tLay->addWidget(m_appTitle);
    tLay->addWidget(m_appSub);
    lay->addWidget(titleBox);

    lay->addStretch(1);

    // Центр: имя документа
    m_docTitle = new QLabel(this);
    m_docTitle->setAlignment(Qt::AlignCenter);
    lay->addWidget(m_docTitle, 0, Qt::AlignCenter);

    lay->addStretch(1);

    // Правая группа: кнопки
    m_btnMin   = makeWindowButton(m_theme.svgMinimize, tr("Свернуть"));
    m_btnMax   = makeWindowButton(m_theme.svgMaximize, tr("Восстановить"));
    m_btnClose = makeWindowButton(m_theme.svgClose,    tr("Закрыть"));

    lay->addWidget(m_btnMin);
    lay->addWidget(m_btnMax);
    lay->addWidget(m_btnClose);

    connect(m_btnMin,   &QPushButton::clicked, this, &CustomTitleBar::minimizeRequested);
    connect(m_btnMax,   &QPushButton::clicked, this, &CustomTitleBar::maximizeRestoreRequested);
    connect(m_btnClose, &QPushButton::clicked, this, &CustomTitleBar::closeRequested);
}

QPushButton* CustomTitleBar::makeWindowButton(const QString &svg, const QString &tooltip)
{
    auto *btn = new QPushButton(this);
    btn->setFlat(true);
    btn->setCursor(Qt::ArrowCursor);
    btn->setToolTip(tooltip);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setIcon(renderSvgIcon(svg, m_theme.iconSize));
    btn->setIconSize(QSize(m_theme.iconSize, m_theme.iconSize));
    btn->setFixedSize(m_theme.buttonWidth, m_theme.buttonHeight);

    // Стилизация — через stylesheet, чтобы hover/pressed работали "из коробки"
    btn->setStyleSheet(QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            padding: 0;
        }
        QPushButton:hover {
            background-color: %1;
        }
        QPushButton:pressed {
            background-color: %2;
        }
    )")
                           .arg(m_theme.btnHover.name(), m_theme.btnPressed.name()));

    return btn;
}

void CustomTitleBar::setButtonSvg(QPushButton *btn, const QString &svg, const QColor &color)
{
    Q_UNUSED(color);
    btn->setIcon(renderSvgIcon(svg, m_theme.iconSize));
    btn->setIconSize(QSize(m_theme.iconSize, m_theme.iconSize));
}

void CustomTitleBar::setTheme(const TitleBarTheme &theme)
{
    m_theme = theme;
    applyTheme();
}

void CustomTitleBar::applyTheme()
{
    setFixedHeight(m_theme.height);

    // Логотип
    m_logo->setPixmap(renderSvgIcon(m_theme.svgLogo, 20).pixmap(20, 20));

    // Тексты
    QFont fTitle(m_theme.fontFamily, m_theme.titleFontSize, QFont::DemiBold);
    QFont fSub  (m_theme.fontFamily, m_theme.subtitleFontSize, QFont::Normal);

    // --- appTitle ---
    m_appTitle->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appTitle->setAttribute(Qt::WA_NoSystemBackground, true);
    m_appTitle->setAutoFillBackground(false);
    m_appTitle->setFont(fTitle);
    m_appTitle->setStyleSheet(QString(
                                  "QLabel {"
                                  "  color: %1;"
                                  "  background: transparent;"
                                  "  border: none;"
                                  "}"
                                  ).arg(m_theme.textPrimary.name()));

    // --- appSub ---
    m_appSub->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appSub->setAttribute(Qt::WA_NoSystemBackground, true);
    m_appSub->setAutoFillBackground(false);
    m_appSub->setFont(fSub);
    m_appSub->setStyleSheet(QString(
                                "QLabel {"
                                "  color: %1;"
                                "  background: transparent;"
                                "  border: none;"
                                "}"
                                ).arg(m_theme.textSecondary.name()));

    // --- docTitle ---
    m_docTitle->setAttribute(Qt::WA_TranslucentBackground, true);
    m_docTitle->setAttribute(Qt::WA_NoSystemBackground, true);
    m_docTitle->setAutoFillBackground(false);
    m_docTitle->setFont(QFont(m_theme.fontFamily, m_theme.titleFontSize));
    m_docTitle->setStyleSheet(QString(
                                  "QLabel {"
                                  "  color: %1;"
                                  "  background: transparent;"
                                  "  border: none;"
                                  "}"
                                  ).arg(m_theme.textSecondary.name()));

    // Кнопки
    for (QPushButton *b : {m_btnMin, m_btnMax, m_btnClose}) {
        b->setFixedSize(m_theme.buttonWidth, m_theme.buttonHeight);
    }
    setButtonSvg(m_btnMin,   m_theme.svgMinimize, m_theme.btnIcon);
    setButtonSvg(m_btnMax,   m_theme.svgMaximize, m_theme.btnIcon);
    setButtonSvg(m_btnClose, m_theme.svgClose,    m_theme.btnIcon);

    QString btnRadius = QString::number(m_theme.buttonRadius) + "px";

    m_btnMin->setStyleSheet(QString(R"(
        QPushButton { background: transparent; border: none; border-radius: %1; }
        QPushButton:hover { background-color: %2; }
        QPushButton:pressed { background-color: %3; }
    )").arg(btnRadius, m_theme.btnHover.name(), m_theme.btnPressed.name()));

    m_btnMax->setStyleSheet(m_btnMin->styleSheet());

    // Кнопка закрытия — красная при hover
    m_btnClose->setStyleSheet(QString(R"(
        QPushButton { background: transparent; border: none; border-radius: %1; }
        QPushButton:hover { background-color: %2; }
        QPushButton:pressed { background-color: %3; }
    )").arg(btnRadius, m_theme.btnCloseHover.name(), m_theme.btnClosePressed.name()));


    update();
}

void CustomTitleBar::setDocumentTitle(const QString &text)
{
    m_docTitle->setText(text);
}

void CustomTitleBar::setAppTitle(const QString &title, const QString &subtitle)
{
    m_appTitle->setText(title);
    m_appSub->setText(subtitle);
}

void CustomTitleBar::setMinimizeVisible(bool v) { m_btnMin->setVisible(v); }
void CustomTitleBar::setMaximizeVisible(bool v) { m_btnMax->setVisible(v); }
void CustomTitleBar::setCloseVisible(bool v)    { m_btnClose->setVisible(v); }

void CustomTitleBar::updateMaximizeIcon(bool isMaximized)
{
    setButtonSvg(m_btnMax,
                 isMaximized ? m_theme.svgRestore : m_theme.svgMaximize,
                 m_theme.btnIcon);
    m_btnMax->setToolTip(isMaximized ? tr("Восстановить") : tr("Развернуть"));
}

// ---------------------------------------------------------------------------
//  Отрисовка фона
// ---------------------------------------------------------------------------
void CustomTitleBar::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    if (m_theme.cornerRadius > 0) {
        path.addRoundedRect(rect(), m_theme.cornerRadius, m_theme.cornerRadius);
    } else {
        path.addRect(rect());
    }

    // Лёгкий вертикальный градиент
    QLinearGradient g(0, 0, 0, height());
    g.setColorAt(0.0, m_theme.backgroundTop);
    g.setColorAt(1.0, m_theme.background);
    p.fillPath(path, g);

    // Нижняя граница
    p.setPen(m_theme.borderBottom);
    p.drawLine(0, height() - 1, width(), height() - 1);
}

// ---------------------------------------------------------------------------
//  Перетаскивание окна
// ---------------------------------------------------------------------------
void CustomTitleBar::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        QWidget *w = window();
        // offset курсора относительно верхнего-левого угла окна
        m_dragOffset = e->globalPosition().toPoint() - w->frameGeometry().topLeft();
        m_dragging = true;
        e->accept();
    }
    QWidget::mousePressEvent(e);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *e)
{
    if (m_dragging && (e->buttons() & Qt::LeftButton)) {
        QWidget *w = window();
        // Если окно развёрнуто — при перетаскивании "отрываем" его
        if (w->windowState() & Qt::WindowMaximized) {
            // опционально: можно сделать restore + переместить
        }
        w->move(e->globalPosition().toPoint() - m_dragOffset);
        e->accept();
    }
    QWidget::mouseMoveEvent(e);
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    m_dragging = false;
    QWidget::mouseReleaseEvent(e);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        emit doubleClicked();
        e->accept();
    }
    QWidget::mouseDoubleClickEvent(e);
}
