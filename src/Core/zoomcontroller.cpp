#include "zoomcontroller.h"
#include <cmath>
#include "QDebug"

ZoomController::ZoomController()
    : m_series(Even)
    , m_cellWidth(1.0)
    , m_breakpoint(1.0)
    , m_scrolls(0)
    , m_liveSteps(12)
    , m_cellsToZero(4)
    , m_step(0.0)
    , m_stepPrime(0.0)
{
}

void ZoomController::init(double initialCellWidth, double breakpoint)
{
    m_series      = Even;
    m_scrolls     = 0;
    m_cellWidth   = initialCellWidth;
    m_cellsToZero = 4;
    m_breakpoint  = breakpoint > 0.0 ? breakpoint : m_cellWidth * m_cellsToZero;
    applySeriesSettings();
}

void ZoomController::applySeriesSettings()
{
    switch (m_series) {
    case Even:
        m_liveSteps   = 12;
        m_cellsToZero = 4;
        m_step        = m_breakpoint / 24.0;
        m_del         = 2.0;
        break;
    case Integer:
        m_liveSteps   = 11;
        m_cellsToZero = 5;
        m_step        = m_breakpoint / 22.0;
        m_del         = 2.5;
        break;
    case Step5:
        m_liveSteps   = 15;
        m_cellsToZero = 5;
        m_step        = m_breakpoint * 3.0 / 75.0;
        m_del         = 2.0;
        break;
    }

    m_stepPrime   = m_step / m_del;
}

void ZoomController::scroll(int direction)
{
    if (direction == 0) return;

    const int sign = (direction > 0) ? -1 : 1;

    // Приращение ширины клетки
    m_cellWidth += sign * m_stepPrime;
    m_scrolls   += sign;

    if (m_scrolls >= m_liveSteps) {
        switch (m_series) {
            case Even:    m_series = Integer;         break;
            case Integer: m_series = Step5;  m_exp--; break;
            case Step5:   m_series = Even;            break;
        }
        applySeriesSettings();
        m_cellWidth = m_breakpoint / m_cellsToZero;
        m_scrolls = 0;
    }
    else if (m_scrolls < 0) {
        switch (m_series) {
            case Even:    m_series = Step5;   m_scrolls = 14;           break;
            case Integer: m_series = Even;    m_scrolls = 11;           break;
            case Step5:   m_series = Integer; m_scrolls = 10; m_exp++;  break;
        }
        applySeriesSettings();

        m_cellWidth = m_breakpoint / m_del - m_stepPrime;
    }
}

int ZoomController::seriesStep() const {
    return m_series == Integer ? 1 : m_series == Even ? 2 : 5;
}

QString ZoomController::toString() const
{
    QString sName;
    switch (m_series) {
        case Even:    sName = "Even";    break;
        case Integer: sName = "Integer"; break;
        case Step5:   sName = "Step5";   break;
    }

    return QString("series=%1  cellWidth=%2  scrolls=%3/%4  bp=%5  step=%6  step'=%7")
        .arg(sName)
        .arg(m_cellWidth,   0, 'g', 10)
        .arg(m_scrolls)
        .arg(m_liveSteps)
        .arg(m_breakpoint,  0, 'g', 10)
        .arg(m_step,        0, 'g', 10)
        .arg(m_stepPrime,   0, 'g', 10);
}
