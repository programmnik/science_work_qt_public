#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace ScreenConfig {
const int DEFAULT_WIDTH = 1200;
const int DEFAULT_HEIGHT = 800;
const int FPS = 60;
const QString WINDOW_TITLE = "СТЕРЕОМЕТРИКС";
}

namespace CameraConfig {
    const float MOVE_SPEED = 0.3f;
    const float ROTATION_SPEED = 0.02f;
    const float DEFAULT_X = 0.0f;
    const float DEFAULT_Y = -1.5f;
    const float DEFAULT_Z = 1.2f;
    const float DEFAULT_ROT_X = 65.0f;
    const float DEFAULT_ROT_Y = 0.0f;
    const float DEFAULT_ROT_Z = 0.0f;
}

namespace MouseConfig {
const float SENSITIVITY = 0.4f;
const float ZOOM_SENSITIVITY = 0.001f;
const float MIN_ZOOM = 0.3f;
const float MAX_ZOOM = 3.0f;
}

namespace SphereConfig {
const float RADIUS = 0.5f;
const int RINGS_COUNT = 20;
const int SECTORS_COUNT = 20;
}

namespace FPSConfig {
const int TARGET_FPS = 60;           // Целевой FPS
const int MIN_FPS = 10;              // Минимальный FPS (ниже не опускаемся)
const int MAX_FRAME_SKIP = 5;        // Максимальное количество пропускаемых кадров
const float UPDATE_INTERVAL_MS = 16.67f; // 1000/60 ≈ 16.67ms

// Коэффициенты для ускорения при пропуске кадров
const float MAX_SPEED_MULTIPLIER = 5.0f;  // Максимальный множитель скорости
const float SPEED_MULTIPLIER_STEP = 0.1f; // Шаг изменения множителя
}

namespace Colors {
const float BACKGROUND_R = 0.1f;
const float BACKGROUND_G = 0.1f;
const float BACKGROUND_B = 0.2f;

const float PLANE_R = 0.2f;
const float PLANE_G = 0.4f;
const float PLANE_B = 0.6f;

const float AXIS_X_R = 1.0f;
const float AXIS_X_G = 0.0f;
const float AXIS_X_B = 0.0f;

const float AXIS_Y_R = 0.0f;
const float AXIS_Y_G = 1.0f;
const float AXIS_Y_B = 0.0f;

const float SPHERE_R = 0.3f;
const float SPHERE_G = 0.6f;
const float SPHERE_B = 0.9f;
}

#endif // CONSTANTS_H
