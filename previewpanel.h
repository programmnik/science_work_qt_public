#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>

class PreviewStack : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewStack(const QString &title, const QStringList &imagePaths,
        const QStringList &captions, const QStringList &videoPaths, QWidget *parent = nullptr);

    QString getVideoPath(int index) const {return m_videoPaths[index];}
    QString getCaption(int index) const {return m_captions[index];}

signals:
    void imageClicked(int index, const QString &videoPath, const QString &caption);

private slots:
    void onPrevClicked();
    void onNextClicked();
    void onImageClicked();
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void updateImage(int index);

    QString m_title;
    QStringList m_imagePaths;
    QStringList m_captions;
    QStringList m_videoPaths;
    int m_currentIndex;

    QLabel *m_titleLabel;
    QLabel *m_imageLabel;
    QLabel *m_captionLabel;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
};

class PreviewPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewPanel(QWidget *parent = nullptr);
    ~PreviewPanel();

private:
    void setupUI();
    void setupStacks(QVBoxLayout *stacksLayout);
    void setupPlayer();
    void showPlayer(const QString &videoPath, const QString &caption);
    void showStacks();

private slots:
    void onBackClicked();
    void onImageClicked(int index, const QString &videoPath, const QString &caption);

private:
    QWidget *m_stacksView;
    QWidget *m_playerView;
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QLabel *m_playerCaption;
    QPushButton *m_backButton;
};

#endif // PREVIEWPANEL_H
