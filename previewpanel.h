#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScrollArea>
#include <QEnterEvent>
#include <QPainterPath>
#include <QCryptographicHash>
#include <QResizeEvent>
#include <QStringList>
#include <QSlider>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QMouseEvent>

class ModernButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
public:
    explicit ModernButton(const QString &text, QWidget *parent = nullptr);
    int opacity() const { return m_opacity; }
    void setOpacity(int opacity) { m_opacity = opacity; update(); }
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    int m_opacity = 255;
    QPropertyAnimation *m_animation;
};

class BackgroundWidget : public QWidget
{
public:
    explicit BackgroundWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

class VideoControls : public QWidget
{
    Q_OBJECT
public:
    explicit VideoControls(QWidget *parent = nullptr);
    void setDuration(qint64 duration);
    void setPosition(qint64 position);
    void setPlaying(bool playing);
    void setVolume(int volume);
    void showControls();
    void hideControls();
    void setFullscreen(bool fullscreen);
    void setLoading(bool loading);
signals:
    void playPauseClicked();
    void stopClicked();
    void positionChanged(qint64 position);
    void volumeChanged(int volume);
    void fullscreenClicked();
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;  // <-- ДОБАВИТЬ ЭТУ СТРОКУ
private slots:
    void onSliderMoved(int value);
    void onSliderPressed();
    void onSliderReleased();
    void onAutoHideTimeout();
    void onPlayPauseClicked();
    void onFullscreenClicked();
private:
    void setupUI();
    void updatePlayPauseButton();
    QString formatTime(qint64 milliseconds);
    void startAutoHideTimer();
    void stopAutoHideTimer();
    QWidget *m_controlsContainer;
    QPushButton *m_playPauseButton;
    QPushButton *m_stopButton;
    QPushButton *m_fullscreenButton;
    QSlider *m_positionSlider;
    QSlider *m_volumeSlider;
    QLabel *m_timeLabel;
    QLabel *m_volumeLabel;
    QLabel *m_loadingLabel;
    QTimer *m_autoHideTimer;
    bool m_isDragging;
    bool m_isPlaying;
    qint64 m_duration;
    bool m_controlsVisible;
    int m_volume;
    bool m_isFullscreen;
    bool m_isLoading;
};

class PreviewStack : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewStack(const QString &title, const QStringList &imagePaths,
                          const QStringList &captions, const QStringList &videoPaths,
                          const QStringList &descriptions, QWidget *parent = nullptr);
signals:
    void imageClicked(int index, const QString &videoPath, const QString &caption, const QString &description);
private slots:
    void onPrevClicked();
    void onNextClicked();
    void onImageClicked();
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    void updateImage(int index);
    void setupNavigationDots();
    QPixmap loadRoundedImage(const QString &path, int width, int height, int radius);
    QString getCachePath(const QString &originalPath, int width, int height, int radius);
    QString m_title;
    QStringList m_imagePaths;
    QStringList m_captions;
    QStringList m_videoPaths;
    QStringList m_descriptions;
    int m_currentIndex;
    QLabel *m_titleLabel;
    QLabel *m_imageLabel;
    QLabel *m_captionTitle;
    QLabel *m_captionDescription;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QHBoxLayout *m_dotsLayout;
    QList<QLabel*> m_dots;
    QString m_cacheDir;
};

class PreviewPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewPanel(QWidget *parent = nullptr);
    ~PreviewPanel();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;  // <-- ДОБАВИТЬ ЭТУ СТРОКУ
    void keyPressEvent(QKeyEvent *event) override;
private:
    void setupUI();
    void setupStacks(QVBoxLayout *stacksLayout);
    void setupPlayer();
    void showPlayer(const QString &videoPath, const QString &caption, const QString &description, const QString &previewPath);
    void showStacks();
    void toggleFullscreen();
private slots:
    void onBackClicked();
    void onImageClicked(int index, const QString &videoPath, const QString &caption, const QString &description);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlayPauseClicked();
    void onStopClicked();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onFullscreenClicked();
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
private:
    QWidget *m_stacksView;
    QWidget *m_playerView;
    QStackedWidget *m_mainStack;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QVideoWidget *m_videoWidget;
    QLabel *m_playerTitle;
    QLabel *m_playerPreview;
    ModernButton *m_backButton;
    BackgroundWidget *m_backgroundWidget;
    VideoControls *m_videoControls;
    QWidget *m_videoContainer;
    QStackedWidget *m_videoStack;
    QStringList m_imagePaths;
    bool m_isFullscreen;
    bool m_isVideoReady;
    QTimer *m_loadTimer;
};

#endif // PREVIEWPANEL_H
