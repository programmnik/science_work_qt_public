#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QColor>
#include <QMap>
#include <QPair>

class ToggleButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)

public:
    struct State {
        QString text;
        QColor color;
        QColor hoverColor;
        QColor pressedColor;
        QColor borderColor;
        QColor textColor;
        QVariant userData;
    };

    explicit ToggleButton(QWidget *parent = nullptr);
    ~ToggleButton();

    // Настройка состояний
    void addState(const QString& name, const State& state);
    void removeState(const QString& name);
    void clearStates();

    // Переключение
    void setState(const QString& name);
    void setStateByIndex(int index);
    void toggle();
    QString currentStateName() const { return m_currentStateName; }
    int currentStateIndex() const { return m_currentStateIndex; }

    // Исправленный метод — возвращаем копию, а не ссылку
    State currentState() const {
        if (m_states.contains(m_currentStateName)) {
            return m_states[m_currentStateName];
        }
        return State();
    }

    // Доступ к состояниям
    bool hasState(const QString& name) const;
    QStringList stateNames() const { return m_states.keys(); }
    int stateCount() const { return m_states.size(); }

    // Настройки внешнего вида
    void setFixedSize(int w, int h);
    void setBorderRadius(int radius) { m_borderRadius = radius; }
    void setFontSize(int size) { m_fontSize = size; }
    void setOpacity(int opacity) { m_opacity = opacity; update(); }
    int opacity() const { return m_opacity; }

    // Получение данных
    QVariant getUserData() const {
        if (m_states.contains(m_currentStateName)) {
            return m_states[m_currentStateName].userData;
        }
        return QVariant();
    }
    template<typename T> T getUserData() const {
        return getUserData().value<T>();
    }

signals:
    void stateChanged(const QString& oldState, const QString& newState);
    void stateChangedByIndex(int oldIndex, int newIndex);
    void toggled(bool isOn);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupUI();
    void applyState(const State& state);

    QMap<QString, State> m_states;
    QString m_currentStateName;
    int m_currentStateIndex = -1;

    // Настройки
    int m_borderRadius = 18;
    int m_fontSize = 13;
    int m_opacity = 255;
    bool m_isPressed = false;
    bool m_isHovered = false;

    // Анимация
    QPropertyAnimation* m_animation = nullptr;
    int m_targetOpacity = 255;
};

#endif // TOGGLEBUTTON_H
