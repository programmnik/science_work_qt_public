#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QListWidget>

class SettingsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPanel(QWidget *parent = nullptr);
    ~SettingsPanel();

signals:
    void settingsChanged();

private slots:
    void onResetClicked();
    void onApplyClicked();
    void onCategoryChanged(int index);

private:
    void setupUI();
    QWidget* createGraphicsPage();
    QWidget* createInterfacePage();
    QWidget* createPerformancePage();
    QWidget* createAboutPage();

    // Список категорий
    QListWidget *m_categoryList;
    QStackedWidget *m_contentStack;
    QLabel *m_pageTitle;
    QLabel *m_pageSubtitle;

    // Графика
    QSlider *m_fovSlider;
    QLabel *m_fovValue;
    QSlider *m_renderDistanceSlider;
    QLabel *m_renderDistanceValue;
    QComboBox *m_qualityCombo;
    QCheckBox *m_vsyncCheck;
    QCheckBox *m_antialiasingCheck;

    // Интерфейс
    QComboBox *m_themeCombo;
    QSlider *m_uiScaleSlider;
    QLabel *m_uiScaleValue;
    QCheckBox *m_showHintsCheck;

    // Производительность
    QSpinBox *m_fpsLimitSpin;
    QCheckBox *m_multithreadingCheck;
    QCheckBox *m_showFpsCheck;
};

#endif // SETTINGSPANEL_H
