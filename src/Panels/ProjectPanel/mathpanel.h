#ifndef MATHPANEL_H
#define MATHPANEL_H

#include <QWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QScrollArea>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QHBoxLayout>
#include <QPointer>

// ============ Сегментный переключатель (универсальный) ============
class SegmentTab : public QWidget
{
    Q_OBJECT
public:
    enum class Orientation { Horizontal, Vertical };
    Q_ENUM(Orientation)

    explicit SegmentTab(const QStringList &items, QWidget *parent = nullptr);

    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);

    void setFontSize(int pt) { m_fontPt = pt; updateGeometry(); update(); }

    Orientation orientation() const { return m_orient; }
    void setOrientation(Orientation o);

    // Размеры одной «кнопки» (ячейки) — используются для min size
    void setItemHeight(int h) { m_itemHeight = h; updateGeometry(); update(); }
    void setItemWidth(int w)  { m_itemWidth  = w; updateGeometry(); update(); }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentChanged(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QStringList m_items;
    int m_currentIndex = 0;
    int m_hoveredIndex = -1;
    int m_fontPt = 11;
    int m_itemHeight = 24;   // желаемая высота одной ячейки (для horiz)
    int m_itemWidth  = 80;   // желаемая ширина одной ячейки (для vert)
    Orientation m_orient = Orientation::Horizontal;
    QList<QRect> m_itemRects;

    void recalcRects();
};

// ============ Кнопка математики ============
class MathButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MathButton(const QString &label, const QString &insertText,
                        bool accent = false, QWidget *parent = nullptr);
    QString insertText() const { return m_insertText; }
    int cursorOffset() const { return m_cursorOffset; }
    void setAccent(bool accent);
private:
    QString m_insertText;
    int m_cursorOffset = -1;
    bool m_accent = false;
};

// ============ Раздел панели ============
class MathSection : public QWidget
{
    Q_OBJECT
public:
    explicit MathSection(QWidget *parent = nullptr);
    void setContent(QWidget *w);
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    QVBoxLayout *m_layout;
    QWidget *m_content = nullptr;
};

// ============ Основная панель ============
class MathPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MathPanel(QWidget *parent = nullptr);
    ~MathPanel();

    void setTargetLineEdit(QLineEdit *edit) { m_targetEdit = edit; }

signals:
    void functionInserted(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onMainSectionChanged(int index);
    void onCategoryChanged(int index);

private:
    struct FuncGroup {
        QString title;
        QList<QPair<QString, QString>> items; // {label, insert}
    };

    QWidget* buildFunctionsPage(const QList<FuncGroup> &groups);
    void rebuildFunctionsSection();
    int m_functionsRows = 3;

    enum class KeyboardLang { Latin, Greek };
    KeyboardLang m_keyboardLang = KeyboardLang::Latin;
    bool m_keyboardShift = false;
    QWidget *m_keyboardContent = nullptr;   // контейнер, который пересобираем

    void rebuildKeyboardContent(bool bigFont);
    QWidget* buildKeyboardRow(const QList<QPair<QString, QString>> &keys,
                              const QList<int> &stretches,
                              bool bigFont);
    QString keyboardLetterAt(int row, int col) const;   // возвращает букву
    void toggleKeyboardLang();
    void toggleKeyboardShift();

    void setupUI();
    QWidget* createMainTabs();
    QWidget* createFunctionsSection();
    QWidget* createCalculatorSection();
    QWidget* createKeyboardSection();

    void insertToTarget(const QString &text, int cursorOffset = -1);

    // Переключение wide/narrow
    void applyLayoutMode(bool wide);
    void rebuildMainLayout();

    QLineEdit *m_targetEdit = nullptr;

    // Вкладки
    SegmentTab *m_mainTabs = nullptr;

    // Layout
    QVBoxLayout *m_rootLayout = nullptr;
    QWidget     *m_mainTabsBar = nullptr;   // бар сверху (в narrow)
    QHBoxLayout *m_contentRow = nullptr;    // [SegmentTab (wide)] + [stack]

    QStackedWidget *m_mainStack = nullptr;
    QStackedWidget *m_categoryStack = nullptr;
    SegmentTab *m_categoryTabs = nullptr;

    QGraphicsOpacityEffect *m_opacityEffect = nullptr;
    QPropertyAnimation *m_fadeAnim = nullptr;

    QList<QString> m_categoryNames;
    bool m_wideMode = false;
    bool m_hasCategoryBar = false;

    QWidget *m_layoutTrash = nullptr;

    QPointer<QWidget> m_lastFocusedWidget;
    void restoreTargetFocus();
};

#endif // MATHPANEL_H
