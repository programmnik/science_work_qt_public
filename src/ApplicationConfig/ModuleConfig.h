#ifndef MODULECONFIG_H
#define MODULECONFIG_H

#include <QMap>
#include <QString>
#include <QVariant>

// Пространства имен для дефолтов каждого модуля
namespace DefaultConfig {

// Общие настройки (shared.cfg)
inline QMap<QString, QVariant> shared() {
    return {
        { "UI/language", QString("ru") },
        { "UI/theme",    QString("dark") }
    };
}

// Приложение (app.cfg)
inline QMap<QString, QVariant> app() {
    return {
        { "Window/width",  1600 },
        { "Window/height", 800 },
        { "Window/title",  QString("СТЕРЕОМЕТРИКС") }
    };
}

// Ядро (core.cfg)
inline QMap<QString, QVariant> core() {
    return {
        { "FPSController/target", 60 },
        { "FPSController/min", 10 },
        { "FPSController/max_frame_skip", 5 },
        { "FPSController/update_interval_ms", 16.67f },
        { "FPSController/max_speed_multiplier", 5.0f },
        { "FPSController/speed_multiplier_step", 0.1f },
        { "FPSController/move_speed", 1.0f },
        { "FPSController/rotation_speed", 1.0f }
    };
}

// Данные (data.cfg)
inline QMap<QString, QVariant> data() {
    return {
        { "ObjectManager/auto_clear", false },
        { "SceneObject/default_visible", true }
    };
}

// Рендер (opengl.cfg)
inline QMap<QString, QVariant> opengl() {
    return {
        { "GLWidget/background_r", 0.1f },
        { "GLWidget/background_g", 0.1f },
        { "GLWidget/background_b", 0.2f },
        { "GLWidget/background_a", 1.0f },
        { "GLWidget/plane_r", 0.2f },
        { "GLWidget/plane_g", 0.4f },
        { "GLWidget/plane_b", 0.6f },
        { "GLWidget/axis_x_r", 1.0f },
        { "GLWidget/axis_x_g", 0.0f },
        { "GLWidget/axis_x_b", 0.0f },
        { "GLWidget/axis_y_r", 0.0f },
        { "GLWidget/axis_y_g", 1.0f },
        { "GLWidget/axis_y_b", 0.0f },
        { "GLWidget/sphere_r", 0.3f },
        { "GLWidget/sphere_g", 0.6f },
        { "GLWidget/sphere_b", 0.9f }
    };
}

// UI-примитивы (ui.cfg)
inline QMap<QString, QVariant> ui() {
    return {
        { "Camera/move_speed", 0.1f },
        { "Camera/rotation_speed", 0.02f },
        { "Camera/default_x", 0.0f },
        { "Camera/default_y", -1.5f },
        { "Camera/default_z", 1.2f },
        { "Camera/default_rot_x", 65.0f },
        { "Camera/default_rot_y", 0.0f },
        { "Camera/default_rot_z", 0.0f },
        { "Camera/sensitivity", 0.4f },
        { "Camera/zoom_sensitivity", 0.001f },
        { "Camera/min_zoom", 0.3f },
        { "Camera/max_zoom", 3.0f },
        { "Sphere/radius", 0.5f },
        { "Sphere/rings", 20 },
        { "Sphere/sectors", 20 }
    };
}

// Панели (panels.cfg)
inline QMap<QString, QVariant> panels() {
    return {
        { "ProjectPanel/show_docks", true },
        { "MathPanel/font_size", 12 }
    };
}
}

#endif // MODULECONFIG_H
