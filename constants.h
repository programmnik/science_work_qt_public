#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace ScreenConfig {
    const int DEFAULT_WIDTH = 1200;
    const int DEFAULT_HEIGHT = 800;
    const int FPS = 60;
    const QString WINDOW_TITLE = "комплексная плоскость → сфера Римана";
}

namespace CameraConfig {
    const float MOVE_SPEED = 0.3f;
    const float ROTATION_SPEED = 0.02f;
    const float DEFAULT_X= 0.0f;
    const float DEFAULT_Y= -1.5f;
    const float DEFAULT_Z= 1.2f;
    const float DEFAULT_ROT_X= -60.0f;
    const float DEFAULT_ROT_Y= 0.0f;
    const float DEFAULT_ROT_Z= 0.0f;
}

namespace MouseConfig {
    const float SENSITIVITY = 0.4f;
    const float ZOOM_SENSITIVITY = 0.001f;
    const float MIN_ZOOM = 0.3f;
    const float MAX_ZOOM = 3.0f;
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
}

#endif // CONSTANTS_H
