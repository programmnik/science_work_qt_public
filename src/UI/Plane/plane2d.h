#ifndef PLANE2D_H
#define PLANE2D_H

#include "plane.h"
#include "../Camera/cameracore2d.h"
#include "Core/zoomcontroller.h"

class Plane2d : public Plane
{
public:
    Plane2d();
    void draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight) override;

    void setLabelOffsetPercent(float percent) { m_labelOffsetPercent = percent; }

    float getLabelOffsetPercent() const { return m_labelOffsetPercent; }

    // Прокинуть скролл от пользователя в контроллер
    void scroll(int direction) { m_zoomController.scroll(direction); }

    // Доступ к контроллеру (для отладки/настройки)
    ZoomController& zoomController() { return m_zoomController; }
    const ZoomController& zoomController() const { return m_zoomController; }

private:
    QVector<Label> generateLabels(Camera* camera, int screenWidth, int screenHeight) override;
    void drawGrid(CameraCore2D* camera2d, int viewportWidth, int viewportHeight);

    // Вспомогательное: вычислить видимый диапазон в мировых координатах
    void visibleRange(CameraCore2D* cam2d, int screenWidth, int screenHeight,
                      float& minX, float& maxX, float& minY, float& maxY) const;

    float m_labelOffsetPercent = 0.1f;
    ZoomController m_zoomController;

    // Сколько клеток помещается в половину высоты экрана.
    // Определяет визуальную "плотность" сетки.
    static constexpr float kCellsPerHalfHeight = 8.0f;
};

#endif // PLANE2D_H
