#pragma once

#include <QWidget>
#include <QColor>
#include <QString>
#include <QPoint>

class QLabel;
class QPushButton;
class QHBoxLayout;
class QToolButton;

// ---------------------------------------------------------------------------
//  Тема оформления заголовка.  Один экземпляр = один "дизайн".
//  По умолчанию используется TitleBarTheme::defaultDark().
// ---------------------------------------------------------------------------
struct TitleBarTheme
{
    // Геометрия
    int     height           = 32;   // высота всей полосы, px
    int     cornerRadius     = 0;    // радиус скругления (для frameless)
    int     buttonRadius     = 2;    // радиус кнопки px
    int     buttonWidth      = 28;   // ширина кнопки окна
    int     buttonHeight     = 22;   // высота кнопки окна
    int     iconSize         = 16;   // размер svg-иконок кнопок

    // Цвета
    QColor  background       = QColor("#2A2A2E");
    QColor  backgroundTop    = QColor("#33333A");   // для лёгкого градиента
    QColor  borderBottom     = QColor("#1A1A1C");
    QColor  textPrimary      = QColor("#E8E8EA");
    QColor  textSecondary    = QColor("#D7D7E0");
    QColor  accent           = QColor("#00B4D8");

    // Кнопки окна
    QColor  btnHover         = QColor("#3A3A44");
    QColor  btnPressed       = QColor("#33333A");
    QColor  btnCloseHover    = QColor("#F87171");
    QColor  btnClosePressed  = QColor("#E05252");
    QColor  btnIcon          = QColor("#A0A0A8");
    QColor  btnIconHover     = QColor("#E8E8EA");
    QColor  btnCloseIconHov  = QColor("#FFFFFF");

    // Шрифты
    QString fontFamily       = "Inter";
    int     titleFontSize    = 11;
    int     subtitleFontSize = 10;

    // Тексты (могут переопределяться извне)
    QString appTitle         = "Стереометрикс CAD/CAE";
    QString appSubtitle      = "v2.4.1 (x86_64 Qt6)";

    // SVG-иконки (строки, чтобы легко менять извне)
    QString svgLogo;
    QString svgMinimize;
    QString svgMaximize;
    QString svgRestore;
    QString svgClose;

    static TitleBarTheme defaultDark();
};

// ---------------------------------------------------------------------------
//  Кастомный заголовок окна.
// ---------------------------------------------------------------------------
class CustomTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    void setTheme(const TitleBarTheme &theme);
    const TitleBarTheme& theme() const { return m_theme; }

    // Заголовок по центру (имя документа / проекта)
    void setDocumentTitle(const QString &text);
    // Левая часть: "Стереометрикс CAD/CAE" + версия
    void setAppTitle(const QString &title, const QString &subtitle = QString());

    // Управление кнопками
    void setMinimizeVisible(bool v);
    void setMaximizeVisible(bool v);
    void setCloseVisible(bool v);
    void updateMaximizeIcon(bool isMaximized);

signals:
    void minimizeRequested();
    void maximizeRestoreRequested();
    void closeRequested();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    void buildUi();
    void applyTheme();
    QPushButton* makeWindowButton(const QString &svg, const QString &tooltip);
    void setButtonSvg(QPushButton *btn, const QString &svg, const QColor &color);

    TitleBarTheme m_theme;

    QLabel      *m_logo       = nullptr;
    QLabel      *m_appTitle   = nullptr;
    QLabel      *m_appSub     = nullptr;
    QLabel      *m_docTitle   = nullptr;

    QPushButton *m_btnMin     = nullptr;
    QPushButton *m_btnMax     = nullptr;
    QPushButton *m_btnClose   = nullptr;

    // drag
    bool    m_dragging       = false;
    QPoint  m_dragOffset;
};
