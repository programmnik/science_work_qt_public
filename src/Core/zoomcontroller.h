#ifndef ZOOMCONTROLLER_H
#define ZOOMCONTROLLER_H

#include <QString>

class ZoomController
{
public:
    enum Series {
        Even    = 0,   // ряд с шагом 2
        Integer = 1,   // ряд с шагом 1
        Step5   = 2    // ряд с шагом 5
    };

    ZoomController();

    // Инициализация: начальная ширина клетки и точка перелома.
    // Если breakpoint <= 0, вычисляется как cellWidth * cellsToZero.
    void init(double initialCellWidth, double breakpoint = 0.0);

    // Один скролл: direction > 0 — zoom in, < 0 — zoom out.
    void scroll(int direction);

    // Текущее состояние
    double cellWidth() const { return m_cellWidth; }
    double breakpoint() const { return m_breakpoint; }
    int seriesStep() const;
    int scrollsInSeries() const { return m_scrolls; }

    // Константы текущего ряда
    int liveSteps() const { return m_liveSteps; }
    int cellsToZero() const { return m_cellsToZero; }

    int64_t currentExponent() const { return m_exp; }

    QString toString() const;

private:
    void applySeriesSettings();

    Series m_series;
    double m_cellWidth;
    double m_breakpoint;
    int    m_scrolls;

    // Кэш настроек текущего ряда
    int    m_liveSteps;
    int    m_cellsToZero;
    double m_step;    // приращение "гипотетической точки" за скролл
    double m_stepPrime; // приращение cellWidth за скролл
    double m_del;
    int64_t m_exp = 0;
};

#endif // ZOOMCONTROLLER_H
