#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QHash>
#include <QVariant>
#include <QList>

class AppConfig
{
public:
    explicit AppConfig(const QString& defaultManifestPath);
    ~AppConfig() = default;

    /// Загрузить конфигурацию (и при необходимости развернуть из ресурсов).
    bool load();

    // --- Геттеры ---
    bool    contains  (const QString& key) const;
    int     getInt    (const QString& key, int defaultValue = 0) const;
    float   getFloat  (const QString& key, float defaultValue = 0.0f) const;
    double  getDouble (const QString& key, double defaultValue = 0.0) const;
    bool    getBool   (const QString& key, bool defaultValue = false) const;
    QString getString (const QString& key, const QString& defaultValue = {}) const;

    // --- Сеттеры (для страницы Настроек) ---
    void set(const QString& key, const QVariant& value);

    /// Сохранить изменения на диск. Запишет только то, что было изменено через set().
    void sync();

private:
    struct ModuleInfo {
        QString name;
        QString path;
    };

    bool loadManifest(const QString& path);
    void deployResourcesIfNeeded();
    void ensureModuleConfigExists(const QString& moduleName, const QString& filePath);
    void loadModuleConfig(const QString& moduleName, const QString& filePath);

    QString m_manifestPath;
    QString m_baseDir;
    QString m_writeDir; // Папка, куда мы имеем право писать (рядом с бинарником)
    QList<ModuleInfo> m_modules;

    QHash<QString, QVariant> m_data;     // Все текущие настройки
    QHash<QString, QVariant> m_modified; // Только изменённые (для sync)
};

#endif // APPCONFIG_H
