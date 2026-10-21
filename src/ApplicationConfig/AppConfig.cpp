#include "AppConfig.h"
#include "ModuleConfig.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QVariant>
#include <QString>
#include <cmath>

// Вспомогательная функция для красивого сохранения float
// (чтобы в .cfg не было 0.10000000149011612)
static QVariant formatVariantForSettings(const QVariant& var)
{
    if (var.userType() == QMetaType::Float) {
        float f = var.toFloat();
        // Если это целое число — сохраняем как int
        if (std::abs(f - std::round(f)) < 1e-6) {
            return static_cast<int>(std::round(f));
        }
        // Иначе сохраняем как строку с 6 значащими цифрами (убирает хвосты)
        return QString::number(f, 'g', 6);
    }
    return var;
}

AppConfig::AppConfig(const QString& defaultManifestPath)
    : m_manifestPath(defaultManifestPath)
{
    // Папка, где лежит бинарник (там мы будем создавать рабочую папку config)
    m_writeDir = QCoreApplication::applicationDirPath() + "/config";
}

bool AppConfig::load()
{
    // 1. Разворачиваем ресурсы, если папки config рядом с бинарником нет
    deployResourcesIfNeeded();

    // 2. Манифест теперь ищем в рабочей папке (или в ресурсах, если не нашли)
    QString actualManifest = m_writeDir + "/manifest.json";
    if (!QFileInfo::exists(actualManifest)) {
        actualManifest = m_manifestPath; // fallback на переданный путь (ресурсы)
    }

    if (!loadManifest(actualManifest)) {
        qWarning() << "AppConfig: Не удалось загрузить манифест:" << actualManifest;
        return false;
    }

    // 3. Читаем конфиги модулей
    for (const auto& mod : m_modules) {
        QString fullPath = m_writeDir + "/" + mod.path;
        ensureModuleConfigExists(mod.name, fullPath);
        loadModuleConfig(mod.name, fullPath);
    }

    return true;
}

void AppConfig::deployResourcesIfNeeded()
{
    // Если папка конфига уже есть — ничего не делаем
    if (QDir(m_writeDir).exists()) {
        return;
    }

    qInfo() << "AppConfig: Первый запуск или конфиг удален. Разворачиваем из ресурсов.";
    QDir().mkpath(m_writeDir);

    // В ресурсах у нас должна лежать вся папка config
    QFile::copy(":/config/manifest.json", m_writeDir + "/manifest.json");

    // Остальные файлы создадутся автоматически благодаря ensureModuleConfigExists
}

bool AppConfig::loadManifest(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return false;

    QJsonArray modules = doc.object()["modules"].toArray();
    for (const auto& val : modules) {
        QJsonObject obj = val.toObject();
        m_modules.append({ obj["name"].toString(), obj["path"].toString() });
    }
    return true;
}

void AppConfig::ensureModuleConfigExists(const QString& moduleName, const QString& filePath)
{
    QMap<QString, QVariant> defaults;
    if (moduleName == "Shared") defaults = DefaultConfig::shared();
    else if (moduleName == "App") defaults = DefaultConfig::app();
    else if (moduleName == "Core") defaults = DefaultConfig::core();
    else if (moduleName == "Data") defaults = DefaultConfig::data();
    else if (moduleName == "OpenGL") defaults = DefaultConfig::opengl();
    else if (moduleName == "UI") defaults = DefaultConfig::ui();
    else if (moduleName == "Panels") defaults = DefaultConfig::panels();

    if (defaults.isEmpty()) return;

    if (!QFileInfo::exists(filePath)) {
        qInfo() << "AppConfig: Создаем файл конфигурации" << filePath;
        QDir().mkpath(QFileInfo(filePath).absolutePath());

        QSettings settings(filePath, QSettings::IniFormat);
        for (auto it = defaults.begin(); it != defaults.end(); ++it) {
            settings.setValue(it.key(), formatVariantForSettings(it.value())); // Оборачиваем
        }
        settings.sync();
    }
}

void AppConfig::loadModuleConfig(const QString& moduleName, const QString& filePath)
{
    QMap<QString, QVariant> defaults;
    if (moduleName == "Shared") defaults = DefaultConfig::shared();
    else if (moduleName == "App") defaults = DefaultConfig::app();
    else if (moduleName == "Core") defaults = DefaultConfig::core();
    else if (moduleName == "Data") defaults = DefaultConfig::data();
    else if (moduleName == "OpenGL") defaults = DefaultConfig::opengl();
    else if (moduleName == "UI") defaults = DefaultConfig::ui();
    else if (moduleName == "Panels") defaults = DefaultConfig::panels();

    QSettings settings(filePath, QSettings::IniFormat);

    // Проходим по дефолтам. Если в файле есть — берем из файла, если нет — берем дефолт и пишем в файл.
    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        QString key = it.key();
        QVariant defaultValue = it.value();
        QString fullKey = moduleName + "/" + key;

        if (settings.contains(key)) {
            m_data.insert(fullKey, settings.value(key));
        } else {
            m_data.insert(fullKey, defaultValue);
            settings.setValue(key, formatVariantForSettings(defaultValue)); // Оборачиваем
        }
    }

    settings.sync();

    // Читаем оставшиеся пользовательские ключи
    for (const QString& key : settings.allKeys()) {
        QString fullKey = moduleName + "/" + key;
        if (!m_data.contains(fullKey)) {
            m_data.insert(fullKey, settings.value(key));
        }
    }
}

bool AppConfig::contains(const QString& key) const {
    return m_data.contains(key);
}

int AppConfig::getInt(const QString& key, int def) const {
    return m_data.value(key, def).toInt();
}

float AppConfig::getFloat(const QString& key, float def) const {
    return m_data.value(key, def).toFloat();
}

double AppConfig::getDouble(const QString& key, double def) const {
    return m_data.value(key, def).toDouble();
}

bool AppConfig::getBool(const QString& key, bool def) const {
    return m_data.value(key, def).toBool();
}

QString AppConfig::getString(const QString& key, const QString& def) const {
    return m_data.value(key, def).toString();
}

void AppConfig::set(const QString& key, const QVariant& value)
{
    m_data.insert(key, value);
    m_modified.insert(key, value);
}

void AppConfig::sync()
{
    if (m_modified.isEmpty()) return;

    for (const auto& mod : m_modules) {
        QString filePath = m_writeDir + "/" + mod.path;
        QSettings settings(filePath, QSettings::IniFormat);
        bool changed = false;

        for (auto it = m_modified.begin(); it != m_modified.end(); ++it) {
            QString fullKey = it.key();
            if (fullKey.startsWith(mod.name + "/")) {
                QString localKey = fullKey.mid(mod.name.length() + 1);
                settings.setValue(localKey, formatVariantForSettings(it.value())); // Оборачиваем
                changed = true;
            }
        }
        if (changed) settings.sync();
    }
    m_modified.clear();
}
