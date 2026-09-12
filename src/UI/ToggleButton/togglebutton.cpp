#include "togglebutton.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QDebug>

ToggleButton::ToggleButton(QWidget *parent)
    : QPushButton(parent)
{
    setupUI();
}

ToggleButton::~ToggleButton() {}

void ToggleButton::setupUI()
{
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);

    m_animation = new QPropertyAnimation(this, "opacity");
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Подключаем сигнал клика
    connect(this, &QPushButton::clicked, this, &ToggleButton::toggle);
}

// ==================== УПРАВЛЕНИЕ СОСТОЯНИЯМИ ====================

void ToggleButton::addState(const QString& name, const State& state)
{
    m_states[name] = state;
    if (m_states.size() == 1) {
        setState(name);
    }
}

void ToggleButton::removeState(const QString& name)
{
    m_states.remove(name);
    if (m_currentStateName == name && !m_states.isEmpty()) {
        setState(m_states.firstKey());
    }
}

void ToggleButton::clearStates()
{
    m_states.clear();
    m_currentStateName.clear();
    m_currentStateIndex = -1;
}

bool ToggleButton::hasState(const QString& name) const
{
    return m_states.contains(name);
}

// ==================== ПЕРЕКЛЮЧЕНИЕ ====================

void ToggleButton::setState(const QString& name)
{
    if (!m_states.contains(name)) {
        qDebug() << "ToggleButton: State" << name << "not found";
        return;
    }

    QString oldState = m_currentStateName;
    int oldIndex = m_currentStateIndex;

    m_currentStateName = name;
    m_currentStateIndex = m_states.keys().indexOf(name);

    applyState(m_states[name]);
    update();

    emit stateChanged(oldState, name);
    emit stateChangedByIndex(oldIndex, m_currentStateIndex);
    emit toggled(m_currentStateIndex == 1);  // Для совместимости
}

void ToggleButton::setStateByIndex(int index)
{
    if (index < 0 || index >= m_states.size()) return;
    setState(m_states.keys()[index]);
}

void ToggleButton::toggle()
{
    if (m_states.isEmpty()) return;

    QStringList names = m_states.keys();
    int currentPos = names.indexOf(m_currentStateName);
    int nextPos = (currentPos + 1) % names.size();

    setState(names[nextPos]);
}

// ==================== ПРИМЕНЕНИЕ СТИЛЯ ====================

void ToggleButton::applyState(const State& state)
{
    setText(state.text);

    // Формируем стиль
    QString style = QString(R"(
        QPushButton {
            background-color: rgba(%1, %2, %3, %4);
            color: rgb(%5, %6, %7);
            border: 0px solid rgba(%8, %9, %10, %11);
            border-radius: %12px;
            font-size: %13px;
            font-weight: bold;
            font-family: 'Segoe UI', sans-serif;
        }
        QPushButton:hover {
            background-color: rgba(%14, %15, %16, %17);
            border-color: rgba(%18, %19, %20, %21);
        }
        QPushButton:pressed {
            background-color: rgba(%22, %23, %24, %25);
        }
    )")
                        .arg(state.color.red()).arg(state.color.green()).arg(state.color.blue()).arg(state.color.alpha())
                        .arg(state.textColor.red()).arg(state.textColor.green()).arg(state.textColor.blue())
                        .arg(state.borderColor.red()).arg(state.borderColor.green()).arg(state.borderColor.blue()).arg(state.borderColor.alpha())
                        .arg(m_borderRadius)
                        .arg(m_fontSize)
                        .arg(state.hoverColor.red()).arg(state.hoverColor.green()).arg(state.hoverColor.blue()).arg(state.hoverColor.alpha())
                        .arg(state.borderColor.red()).arg(state.borderColor.green()).arg(state.borderColor.blue()).arg(state.borderColor.alpha() * 0.6)
                        .arg(state.pressedColor.red()).arg(state.pressedColor.green()).arg(state.pressedColor.blue()).arg(state.pressedColor.alpha());

    setStyleSheet(style);
}

// ==================== НАСТРОЙКИ ВНЕШНЕГО ВИДА ====================

void ToggleButton::setFixedSize(int w, int h)
{
    QPushButton::setFixedSize(w, h);
}

// ==================== СОБЫТИЯ ====================

void ToggleButton::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = true;
    m_animation->setStartValue(m_opacity);
    m_animation->setEndValue(255);
    m_animation->start();
}

void ToggleButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = false;
    m_animation->setStartValue(m_opacity);
    m_animation->setEndValue(220);
    m_animation->start();
}

void ToggleButton::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
    QPushButton::mousePressEvent(event);
}

void ToggleButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    QPushButton::mouseReleaseEvent(event);
}

void ToggleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем стандартную кнопку
    QStyleOptionButton option;
    option.initFrom(this);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);

    // Рисуем текст поверх (если стандартная отрисовка не показывает текст)
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, text());
}
