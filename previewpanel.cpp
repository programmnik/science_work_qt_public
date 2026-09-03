#include "previewpanel.h"
#include <QPainter>
#include <QLinearGradient>
#include <QFile>
#include <QUrl>
#include <QPixmap>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QShowEvent>

// ===================== BackgroundWidget =====================

BackgroundWidget::BackgroundWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(26, 26, 46));
    gradient.setColorAt(0.5, QColor(22, 33, 62));
    gradient.setColorAt(1, QColor(15, 15, 26));
    painter.fillRect(rect(), gradient);

    int spacing = 80;
    QPen pen(QColor(255, 255, 255, 10));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    for (int x = 0; x < width() + spacing; x += spacing) {
        for (int y = 0; y < height() + spacing; y += spacing) {
            QPolygonF diamond;
            diamond << QPointF(x, y - 30) << QPointF(x + 30, y)
                    << QPointF(x, y + 30) << QPointF(x - 30, y);
            painter.drawPolygon(diamond);
            QPolygonF innerDiamond;
            innerDiamond << QPointF(x, y - 15) << QPointF(x + 15, y)
                         << QPointF(x, y + 15) << QPointF(x - 15, y);
            painter.setPen(QColor(255, 255, 255, 6));
            painter.drawPolygon(innerDiamond);
            painter.setBrush(QColor(255, 255, 255, 12));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(x, y), 2, 2);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
        }
    }
    painter.setPen(QColor(255, 255, 255, 4));
    painter.drawLine(0, height() * 0.3, width(), height() * 0.3);
    painter.drawLine(0, height() * 0.7, width(), height() * 0.7);
}

// ===================== ModernButton =====================

ModernButton::ModernButton(const QString &text, QWidget *parent) : QPushButton(text, parent)
{
    setFlat(true);
    setCursor(Qt::PointingHandCursor);
    setFixedHeight(48);
    setMinimumWidth(120);
    m_animation = new QPropertyAnimation(this, "opacity");
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void ModernButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect();
    QPainterPath path;
    path.addRoundedRect(rect, 12, 12);
    QColor bgColor = isEnabled() ? QColor(13, 115, 119) : QColor(80, 80, 80);
    bgColor.setAlpha(m_opacity);
    painter.fillPath(path, bgColor);
    if (underMouse() && isEnabled()) {
        QColor hoverColor(255, 255, 255, 30);
        painter.fillPath(path, hoverColor);
    }
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(font());
    painter.drawText(rect, Qt::AlignCenter, text());
}

void ModernButton::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    if (isEnabled()) {
        m_animation->setStartValue(m_opacity);
        m_animation->setEndValue(200);
        m_animation->start();
        setCursor(Qt::PointingHandCursor);
    }
}

void ModernButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (isEnabled()) {
        m_animation->setStartValue(m_opacity);
        m_animation->setEndValue(255);
        m_animation->start();
        unsetCursor();
    }
}

// ===================== VideoControls =====================

VideoControls::VideoControls(QWidget *parent) : QWidget(parent)
    , m_isDragging(false), m_isPlaying(false), m_duration(0)
    , m_controlsVisible(true), m_volume(70), m_isFullscreen(false), m_isLoading(false)
{
    setupUI();
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setStyleSheet("background: transparent;");
    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);
    connect(m_autoHideTimer, &QTimer::timeout, this, &VideoControls::onAutoHideTimeout);
    showControls();
}

void VideoControls::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addStretch();

    m_controlsContainer = new QWidget(this);
    m_controlsContainer->setMouseTracking(true);
    m_controlsContainer->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 rgba(0,0,0,0),"
        "        stop:0.5 rgba(0,0,0,0.3),"
        "        stop:1 rgba(0,0,0,0.8));"
        "    border-radius: 12px;"
        "}"
        );

    QVBoxLayout *containerLayout = new QVBoxLayout(m_controlsContainer);
    containerLayout->setContentsMargins(20, 10, 20, 15);
    containerLayout->setSpacing(8);

    // Индикатор загрузки
    m_loadingLabel = new QLabel("Загрузка видео...");
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("color: #ffffff; font-size: 16px; background: transparent;");
    m_loadingLabel->hide();
    containerLayout->addWidget(m_loadingLabel);

    // Прогресс
    QHBoxLayout *progressLayout = new QHBoxLayout();
    progressLayout->setSpacing(10);
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 1000);
    m_positionSlider->setMouseTracking(true);
    m_positionSlider->setEnabled(false);
    m_positionSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: rgba(255,255,255,0.2); border-radius: 2px; }"
        "QSlider::handle:horizontal { background: #0d7377; width: 14px; height: 14px; margin: -5px 0; border-radius: 7px; }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 2px; }"
        "QSlider:hover::handle { background: #14a3a8; width: 16px; height: 16px; margin: -6px 0; }"
        "QSlider:disabled { opacity: 0.5; }"
        );
    connect(m_positionSlider, &QSlider::sliderMoved, this, &VideoControls::onSliderMoved);
    connect(m_positionSlider, &QSlider::sliderPressed, this, &VideoControls::onSliderPressed);
    connect(m_positionSlider, &QSlider::sliderReleased, this, &VideoControls::onSliderReleased);
    progressLayout->addWidget(m_positionSlider);
    m_timeLabel = new QLabel("00:00 / 00:00");
    m_timeLabel->setStyleSheet("color: #ffffff; font-size: 12px; font-family: monospace;");
    m_timeLabel->setMinimumWidth(100);
    progressLayout->addWidget(m_timeLabel);
    containerLayout->addLayout(progressLayout);

    // Кнопки
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(12);
    m_playPauseButton = new QPushButton("▶");
    m_playPauseButton->setFixedSize(36, 36);
    m_playPauseButton->setCursor(Qt::PointingHandCursor);
    m_playPauseButton->setEnabled(false);
    m_playPauseButton->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,0.15); border: none; border-radius: 18px; color: #ffffff; font-size: 16px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.25); }"
        "QPushButton:disabled { opacity: 0.3; }"
        );
    connect(m_playPauseButton, &QPushButton::clicked, this, &VideoControls::onPlayPauseClicked);
    controlsLayout->addWidget(m_playPauseButton);

    m_stopButton = new QPushButton("■");
    m_stopButton->setFixedSize(36, 36);
    m_stopButton->setCursor(Qt::PointingHandCursor);
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,0.15); border: none; border-radius: 18px; color: #ffffff; font-size: 14px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.25); }"
        "QPushButton:disabled { opacity: 0.3; }"
        );
    connect(m_stopButton, &QPushButton::clicked, this, &VideoControls::stopClicked);
    controlsLayout->addWidget(m_stopButton);
    controlsLayout->addStretch();

    // Громкость
    QLabel *volumeIcon = new QLabel("🔊");
    volumeIcon->setStyleSheet("color: #ffffff; font-size: 14px;");
    controlsLayout->addWidget(volumeIcon);
    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(80);
    m_volumeSlider->setMouseTracking(true);
    m_volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 3px; background: rgba(255,255,255,0.2); border-radius: 1.5px; }"
        "QSlider::handle:horizontal { background: #0d7377; width: 12px; height: 12px; margin: -4.5px 0; border-radius: 6px; }"
        "QSlider::sub-page:horizontal { background: #0d7377; border-radius: 1.5px; }"
        );
    connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        m_volume = value;
        m_volumeLabel->setText(QString::number(value) + "%");
        emit volumeChanged(value);
    });
    controlsLayout->addWidget(m_volumeSlider);
    m_volumeLabel = new QLabel("70%");
    m_volumeLabel->setStyleSheet("color: #ffffff; font-size: 11px; font-family: monospace;");
    m_volumeLabel->setMinimumWidth(30);
    controlsLayout->addWidget(m_volumeLabel);

    m_fullscreenButton = new QPushButton("⛶");
    m_fullscreenButton->setFixedSize(36, 36);
    m_fullscreenButton->setCursor(Qt::PointingHandCursor);
    m_fullscreenButton->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,0.15); border: none; border-radius: 18px; color: #ffffff; font-size: 18px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.25); }"
        );
    connect(m_fullscreenButton, &QPushButton::clicked, this, &VideoControls::onFullscreenClicked);
    controlsLayout->addWidget(m_fullscreenButton);

    containerLayout->addLayout(controlsLayout);
    mainLayout->addWidget(m_controlsContainer);
}

void VideoControls::setLoading(bool loading)
{
    m_isLoading = loading;
    if (loading) {
        m_loadingLabel->show();
        m_positionSlider->setEnabled(false);
        m_playPauseButton->setEnabled(false);
        m_stopButton->setEnabled(false);
    } else {
        m_loadingLabel->hide();
        m_positionSlider->setEnabled(true);
        m_playPauseButton->setEnabled(true);
        m_stopButton->setEnabled(true);
    }
}

void VideoControls::setFullscreen(bool fullscreen)
{
    m_isFullscreen = fullscreen;
}

void VideoControls::setDuration(qint64 duration)
{
    m_duration = duration;
    m_timeLabel->setText("00:00 / " + formatTime(duration));
}

void VideoControls::setPosition(qint64 position)
{
    if (!m_isDragging && m_duration > 0) {
        m_positionSlider->setValue(static_cast<int>((position * 1000) / m_duration));
    }
    m_timeLabel->setText(formatTime(position) + " / " + formatTime(m_duration));
}

void VideoControls::setPlaying(bool playing)
{
    m_isPlaying = playing;
    updatePlayPauseButton();
}

void VideoControls::setVolume(int volume)
{
    m_volume = volume;
    m_volumeSlider->setValue(volume);
    m_volumeLabel->setText(QString::number(volume) + "%");
}

void VideoControls::showControls()
{
    m_controlsVisible = true;
    m_controlsContainer->show();
    startAutoHideTimer();
}

void VideoControls::hideControls()
{
    m_controlsVisible = false;
    m_controlsContainer->hide();
    stopAutoHideTimer();
}

void VideoControls::startAutoHideTimer()
{
    stopAutoHideTimer();
    m_autoHideTimer->start(3000);
}

void VideoControls::stopAutoHideTimer()
{
    if (m_autoHideTimer) m_autoHideTimer->stop();
}

void VideoControls::onAutoHideTimeout()
{
    if (!underMouse() && m_controlsVisible && !m_isDragging) {
        hideControls();
    }
}

void VideoControls::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    if (!m_isLoading) showControls();
}

void VideoControls::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (!m_isDragging && !m_isLoading) startAutoHideTimer();
}

void VideoControls::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!m_isLoading) showControls();
}

void VideoControls::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Обновляем размер контейнера
    if (m_controlsContainer) {
        m_controlsContainer->setFixedWidth(width());
    }
}

void VideoControls::onSliderMoved(int value)
{
    if (m_duration > 0) {
        qint64 pos = (value * m_duration) / 1000;
        m_timeLabel->setText(formatTime(pos) + " / " + formatTime(m_duration));
    }
}

void VideoControls::onSliderPressed()
{
    m_isDragging = true;
    stopAutoHideTimer();
}

void VideoControls::onSliderReleased()
{
    m_isDragging = false;
    if (m_duration > 0) {
        emit positionChanged((m_positionSlider->value() * m_duration) / 1000);
    }
    startAutoHideTimer();
}

void VideoControls::onPlayPauseClicked() { emit playPauseClicked(); }
void VideoControls::onFullscreenClicked() { emit fullscreenClicked(); }

void VideoControls::updatePlayPauseButton()
{
    m_playPauseButton->setText(m_isPlaying ? "⏸" : "▶");
}

QString VideoControls::formatTime(qint64 milliseconds)
{
    if (milliseconds <= 0) return "00:00";
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

// ===================== PreviewStack =====================

PreviewStack::PreviewStack(const QString &title, const QStringList &imagePaths,
                           const QStringList &captions, const QStringList &videoPaths,
                           const QStringList &descriptions, QWidget *parent)
    : QWidget(parent), m_title(title), m_imagePaths(imagePaths), m_captions(captions),
    m_videoPaths(videoPaths), m_descriptions(descriptions), m_currentIndex(0)
{
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/rounded_images/";
    QDir().mkpath(m_cacheDir);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(20);

    // Заголовок
    QWidget *titleContainer = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(16);
    QFrame *accentLine = new QFrame();
    accentLine->setFixedSize(4, 28);
    accentLine->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 2px;");
    titleLayout->addWidget(accentLine);
    m_titleLabel = new QLabel(m_title);
    QFont titleFont = m_titleLabel->font();
    titleFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    titleFont.setWeight(QFont::DemiBold);
    titleFont.setPixelSize(22);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #e8e8e8; letter-spacing: 0.3px;");
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    QLabel *counterLabel = new QLabel(QString("%1 / %2").arg(m_currentIndex + 1).arg(m_imagePaths.size()));
    QFont counterFont = counterLabel->font();
    counterFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    counterFont.setPixelSize(13);
    counterFont.setWeight(QFont::Light);
    counterLabel->setFont(counterFont);
    counterLabel->setStyleSheet("color: #8888a0; letter-spacing: 0.5px;");
    counterLabel->setObjectName("counterLabel");
    titleLayout->addWidget(counterLabel);
    mainLayout->addWidget(titleContainer);

    // Изображение
    QWidget *imageContainer = new QWidget();
    imageContainer->setStyleSheet("background: rgba(255,255,255,0.02); border-radius: 24px; border: 1px solid rgba(255,255,255,0.04);");
    QHBoxLayout *imageLayout = new QHBoxLayout(imageContainer);
    imageLayout->setContentsMargins(8, 8, 8, 8);
    imageLayout->setSpacing(12);
    m_prevButton = new QPushButton();
    m_prevButton->setFixedSize(44, 44);
    m_prevButton->setCursor(Qt::PointingHandCursor);
    m_prevButton->setStyleSheet("QPushButton { background: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.06); border-radius: 12px; color: #c0c0d0; font-size: 18px; font-weight: 300; } QPushButton:hover:enabled { background: rgba(13,115,119,0.25); border-color: rgba(13,115,119,0.4); color: #ffffff; } QPushButton:disabled { opacity: 0.2; }");
    m_prevButton->setText("←");
    connect(m_prevButton, &QPushButton::clicked, this, &PreviewStack::onPrevClicked);
    imageLayout->addWidget(m_prevButton);
    m_imageLabel = new QLabel();
    m_imageLabel->setFixedSize(900, 600);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_imageLabel->setStyleSheet("background: transparent; border: none;");
    m_imageLabel->setCursor(Qt::PointingHandCursor);
    m_imageLabel->installEventFilter(this);
    imageLayout->addWidget(m_imageLabel);
    m_nextButton = new QPushButton();
    m_nextButton->setFixedSize(44, 44);
    m_nextButton->setCursor(Qt::PointingHandCursor);
    m_nextButton->setStyleSheet("QPushButton { background: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.06); border-radius: 12px; color: #c0c0d0; font-size: 18px; font-weight: 300; } QPushButton:hover:enabled { background: rgba(13,115,119,0.25); border-color: rgba(13,115,119,0.4); color: #ffffff; } QPushButton:disabled { opacity: 0.2; }");
    m_nextButton->setText("→");
    connect(m_nextButton, &QPushButton::clicked, this, &PreviewStack::onNextClicked);
    imageLayout->addWidget(m_nextButton);
    mainLayout->addWidget(imageContainer);

    // Точки
    m_dotsLayout = new QHBoxLayout();
    m_dotsLayout->setSpacing(8);
    m_dotsLayout->setAlignment(Qt::AlignCenter);
    setupNavigationDots();
    mainLayout->addLayout(m_dotsLayout);

    // Подпись
    QWidget *captionWrapper = new QWidget();
    captionWrapper->setStyleSheet("background: rgba(0,0,0,0.3); border-radius: 16px; border: 1px solid rgba(255,255,255,0.05); padding: 4px;");
    QVBoxLayout *captionMainLayout = new QVBoxLayout(captionWrapper);
    captionMainLayout->setContentsMargins(20, 12, 20, 12);
    captionMainLayout->setSpacing(4);
    m_captionTitle = new QLabel();
    QFont captionTitleFont = m_captionTitle->font();
    captionTitleFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    captionTitleFont.setWeight(QFont::DemiBold);
    captionTitleFont.setPixelSize(20);
    m_captionTitle->setFont(captionTitleFont);
    m_captionTitle->setStyleSheet("color: #e8e8e8; letter-spacing: 0.3px; background: transparent; padding: 2px 0;");
    m_captionTitle->setWordWrap(true);
    captionMainLayout->addWidget(m_captionTitle);
    m_captionDescription = new QLabel();
    QFont captionDescFont = m_captionDescription->font();
    captionDescFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    captionDescFont.setWeight(QFont::Light);
    captionDescFont.setPixelSize(14);
    m_captionDescription->setFont(captionDescFont);
    m_captionDescription->setStyleSheet("color: #a0a0b8; letter-spacing: 0.2px; background: transparent; padding: 2px 0;");
    m_captionDescription->setWordWrap(true);
    captionMainLayout->addWidget(m_captionDescription);
    mainLayout->addWidget(captionWrapper);
    updateImage(0);
}

QString PreviewStack::getCachePath(const QString &originalPath, int width, int height, int radius)
{
    QString hash = QCryptographicHash::hash(
                       QString("%1_%2_%3_%4").arg(originalPath).arg(width).arg(height).arg(radius).toUtf8(),
                       QCryptographicHash::Md5
                       ).toHex();
    return m_cacheDir + hash + ".png";
}

QPixmap PreviewStack::loadRoundedImage(const QString &path, int width, int height, int radius)
{
    QString cachePath = getCachePath(path, width, height, radius);
    if (QFile::exists(cachePath)) {
        QPixmap cached(cachePath);
        if (!cached.isNull()) return cached;
    }
    QPixmap original(path);
    if (original.isNull()) { qDebug() << "Не удалось загрузить:" << path; return QPixmap(); }
    QPixmap scaled = original.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap result(scaled.size());
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPainterPath clipPath;
    clipPath.addRoundedRect(QRect(0, 0, scaled.width(), scaled.height()), radius, radius);
    painter.setClipPath(clipPath);
    painter.drawPixmap(0, 0, scaled);
    painter.end();
    result.save(cachePath, "PNG");
    return result;
}

void PreviewStack::setupNavigationDots()
{
    QLayoutItem *item;
    while ((item = m_dotsLayout->takeAt(0)) != nullptr) { delete item->widget(); delete item; }
    m_dots.clear();
    for (int i = 0; i < m_imagePaths.size(); ++i) {
        QLabel *dot = new QLabel();
        dot->setFixedSize(8, 8);
        dot->setStyleSheet("background: rgba(255,255,255,0.12); border-radius: 4px; border: none;");
        m_dotsLayout->addWidget(dot);
        m_dots.append(dot);
    }
    if (m_currentIndex >= 0 && m_currentIndex < m_dots.size()) {
        m_dots[m_currentIndex]->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 4px; border: none; min-width: 28px;");
    }
}

void PreviewStack::updateImage(int index)
{
    if (index >= 0 && index < m_imagePaths.size()) {
        m_currentIndex = index;
        QLabel *counter = findChild<QLabel*>("counterLabel");
        if (counter) counter->setText(QString("%1 / %2").arg(m_currentIndex + 1).arg(m_imagePaths.size()));
        QPixmap roundedPixmap = loadRoundedImage(m_imagePaths[index], 900, 600, 24);
        if (!roundedPixmap.isNull()) m_imageLabel->setPixmap(roundedPixmap);
        else { m_imageLabel->setText("Изображение не найдено"); m_imageLabel->setStyleSheet("color: #666; font-size: 14px; background: rgba(20,20,35,0.5); border-radius: 16px;"); }
        m_captionTitle->setText(m_captions[index]);
        m_captionDescription->setText(m_descriptions[index]);
    }
    m_prevButton->setEnabled(m_currentIndex > 0);
    m_nextButton->setEnabled(m_currentIndex < m_imagePaths.size() - 1);
    for (int i = 0; i < m_dots.size(); ++i) {
        if (i == m_currentIndex) m_dots[i]->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 4px; border: none; min-width: 28px;");
        else m_dots[i]->setStyleSheet("background: rgba(255,255,255,0.12); border-radius: 4px; border: none; min-width: 8px;");
    }
}

void PreviewStack::onPrevClicked() { if (m_currentIndex > 0) updateImage(m_currentIndex - 1); }
void PreviewStack::onNextClicked() { if (m_currentIndex < m_imagePaths.size() - 1) updateImage(m_currentIndex + 1); }
void PreviewStack::onImageClicked() { emit imageClicked(m_currentIndex, m_videoPaths[m_currentIndex], m_captions[m_currentIndex], m_descriptions[m_currentIndex]); }
bool PreviewStack::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_imageLabel && event->type() == QEvent::MouseButtonPress) { onImageClicked(); return true; }
    return QWidget::eventFilter(obj, event);
}

// ===================== PreviewPanel =====================

PreviewPanel::PreviewPanel(QWidget *parent) : QWidget(parent), m_isFullscreen(false), m_isVideoReady(false)
{
    m_backgroundWidget = new BackgroundWidget(this);
    m_backgroundWidget->setGeometry(rect());
    m_backgroundWidget->lower();
    setAttribute(Qt::WA_StyledBackground);

    m_loadTimer = new QTimer(this);
    m_loadTimer->setSingleShot(true);
    connect(m_loadTimer, &QTimer::timeout, this, [this]() {
        if (m_mediaPlayer && m_isVideoReady) {
            m_mediaPlayer->play();
            if (m_videoControls) {
                m_videoControls->setPlaying(true);
                m_videoControls->setLoading(false);
                m_videoControls->showControls();
                m_videoControls->raise();
            }
        }
    });

    setupUI();
}

PreviewPanel::~PreviewPanel() { if (m_mediaPlayer) m_mediaPlayer->stop(); }

void PreviewPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_backgroundWidget) m_backgroundWidget->setGeometry(rect());
    if (m_videoControls && m_videoContainer) {
        m_videoControls->setGeometry(0, 0, m_videoContainer->width(), m_videoContainer->height());
        m_videoControls->raise();
        m_videoControls->update();
    }
}

void PreviewPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_videoControls && m_videoContainer) {
        m_videoControls->setGeometry(0, 0, m_videoContainer->width(), m_videoContainer->height());
        m_videoControls->raise();
        m_videoControls->update();
    }
}

void PreviewPanel::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_isFullscreen) toggleFullscreen();
    if (event->key() == Qt::Key_F && m_isFullscreen) toggleFullscreen();
    QWidget::keyPressEvent(event);
}

void PreviewPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 30, 60, 30);
    mainLayout->setSpacing(0);
    m_mainStack = new QStackedWidget(this);
    m_mainStack->setStyleSheet("background: transparent;");
    mainLayout->addWidget(m_mainStack);

    // Stacks View
    m_stacksView = new QWidget();
    m_stacksView->setStyleSheet("background: transparent;");
    QVBoxLayout *stacksViewLayout = new QVBoxLayout(m_stacksView);
    stacksViewLayout->setContentsMargins(0, 0, 0, 0);
    stacksViewLayout->setSpacing(24);

    QWidget *headerWidget = new QWidget();
    headerWidget->setStyleSheet("background: rgba(255,255,255,0.03); border-radius: 20px; border: 1px solid rgba(255,255,255,0.06); padding: 20px 28px;");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    QLabel *headerIcon = new QLabel();
    headerIcon->setFixedSize(32, 32);
    headerIcon->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 10px;");
    headerLayout->addWidget(headerIcon);
    QLabel *headerTitle = new QLabel("Галерея превью");
    QFont headerFont = headerTitle->font();
    headerFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    headerFont.setWeight(QFont::DemiBold);
    headerFont.setPixelSize(24);
    headerTitle->setFont(headerFont);
    headerTitle->setStyleSheet("color: #e8e8e8; letter-spacing: -0.3px;");
    headerLayout->addWidget(headerTitle);
    QLabel *headerSubtitle = new QLabel("Выберите превью для просмотра");
    QFont subFont = headerSubtitle->font();
    subFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    subFont.setPixelSize(14);
    subFont.setWeight(QFont::Light);
    headerSubtitle->setFont(subFont);
    headerSubtitle->setStyleSheet("color: #8888a0; margin-left: 8px; letter-spacing: 0.2px;");
    headerLayout->addWidget(headerSubtitle);
    headerLayout->addStretch();
    stacksViewLayout->addWidget(headerWidget);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; } QScrollBar:vertical { background: rgba(255,255,255,0.03); width: 6px; border-radius: 3px; margin: 4px 0; } QScrollBar::handle:vertical { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0d7377,stop:1 #14a3a8); border-radius: 3px; min-height: 30px; } QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }");
    QWidget *stacksContainer = new QWidget();
    stacksContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *stacksLayout = new QVBoxLayout(stacksContainer);
    stacksLayout->setContentsMargins(0, 0, 0, 0);
    stacksLayout->setSpacing(28);
    setupStacks(stacksLayout);
    stacksLayout->addStretch();
    scrollArea->setWidget(stacksContainer);
    stacksViewLayout->addWidget(scrollArea);
    m_mainStack->addWidget(m_stacksView);

    setupPlayer();
    m_mainStack->addWidget(m_playerView);
}

void PreviewPanel::setupStacks(QVBoxLayout *stacksLayout)
{
    m_imagePaths.clear();
    m_imagePaths << ":/images/preview1.png" << ":/images/preview2.png";
    QString videosDir = QCoreApplication::applicationDirPath() + "/videos/";
    if (!QDir(videosDir).exists()) videosDir = QDir::currentPath() + "/videos/";
    if (!QDir(videosDir).exists()) videosDir = QDir::homePath() + "/videos/";

    QStringList videoPaths, captions, descriptions;
    videoPaths << videosDir + "preview1.mp4" << videosDir + "preview2.mp4";
    captions << "Стереографическая проекция" << "Иллюстрация бесконечности";
    descriptions << "Демонстрация стереографической проекции сферы на плоскость. Визуализация математического преобразования в реальном времени."
                 << "Визуализация концепции бесконечности через фрактальные структуры. Постепенное погружение в бесконечную последовательность.";

    PreviewStack *mainStack = new PreviewStack("Основные превью", m_imagePaths, captions, videoPaths, descriptions);
    connect(mainStack, &PreviewStack::imageClicked, this, &PreviewPanel::onImageClicked);
    stacksLayout->addWidget(mainStack);
}

void PreviewPanel::setupPlayer()
{
    m_playerView = new QWidget();
    m_playerView->setStyleSheet("background: transparent;");
    QVBoxLayout *playerLayout = new QVBoxLayout(m_playerView);
    playerLayout->setContentsMargins(0, 0, 0, 0);
    playerLayout->setSpacing(20);

    // Верхняя панель
    QWidget *topBarWidget = new QWidget();
    topBarWidget->setStyleSheet("background: rgba(255,255,255,0.03); border-radius: 16px; border: 1px solid rgba(255,255,255,0.06); padding: 12px 20px;");
    QHBoxLayout *topBar = new QHBoxLayout(topBarWidget);
    m_backButton = new ModernButton("← Назад в галерею");
    m_backButton->setFixedHeight(40);
    m_backButton->setMinimumWidth(160);
    connect(m_backButton, &ModernButton::clicked, this, &PreviewPanel::showStacks);
    topBar->addWidget(m_backButton);
    QLabel *nowPlayingLabel = new QLabel("Сейчас играет:");
    QFont npFont = nowPlayingLabel->font();
    npFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    npFont.setPixelSize(13);
    npFont.setWeight(QFont::Light);
    nowPlayingLabel->setFont(npFont);
    nowPlayingLabel->setStyleSheet("color: #8888a0; letter-spacing: 0.5px; padding-left: 20px;");
    topBar->addWidget(nowPlayingLabel);
    m_playerTitle = new QLabel();
    QFont titleFont = m_playerTitle->font();
    titleFont.setFamily("Inter, -apple-system, Segoe UI, Roboto, sans-serif");
    titleFont.setWeight(QFont::DemiBold);
    titleFont.setPixelSize(18);
    m_playerTitle->setFont(titleFont);
    m_playerTitle->setStyleSheet("color: #e8e8e8; letter-spacing: 0.3px; padding-left: 6px;");
    m_playerTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    topBar->addWidget(m_playerTitle, 1);
    playerLayout->addWidget(topBarWidget);

    // Видео контейнер
    m_videoContainer = new QWidget();
    m_videoContainer->setStyleSheet("background: rgba(0,0,0,0.4); border-radius: 20px; border: 1px solid rgba(255,255,255,0.06);");
    m_videoContainer->setMouseTracking(true);
    m_videoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Создаем layout для контейнера
    QVBoxLayout *containerLayout = new QVBoxLayout(m_videoContainer);
    containerLayout->setContentsMargins(2, 2, 2, 2);
    containerLayout->setSpacing(0);

    // Стек для превью/видео
    m_videoStack = new QStackedWidget();
    m_videoStack->setStyleSheet("background: transparent;");
    m_videoStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Превью
    QWidget *previewWidget = new QWidget();
    previewWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewWidget);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    m_playerPreview = new QLabel();
    m_playerPreview->setAlignment(Qt::AlignCenter);
    m_playerPreview->setStyleSheet("background: transparent; border: none;");
    m_playerPreview->setScaledContents(true);
    previewLayout->addWidget(m_playerPreview);
    m_videoStack->addWidget(previewWidget);

    // Видео
    QWidget *videoWidget = new QWidget();
    videoWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *videoWidgetLayout = new QVBoxLayout(videoWidget);
    videoWidgetLayout->setContentsMargins(0, 0, 0, 0);
    m_videoWidget = new QVideoWidget();
    m_videoWidget->setStyleSheet("border-radius: 18px; background: #0a0a12;");
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoWidgetLayout->addWidget(m_videoWidget);
    m_videoStack->addWidget(videoWidget);
    m_videoStack->setCurrentIndex(0);

    containerLayout->addWidget(m_videoStack);
    playerLayout->addWidget(m_videoContainer, 1);

    // Контролы - накладываем поверх
    m_videoControls = new VideoControls(m_videoContainer);
    m_videoControls->setGeometry(0, 0, m_videoContainer->width(), m_videoContainer->height());
    m_videoControls->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    m_videoControls->setMouseTracking(true);
    m_videoControls->raise();
    m_videoControls->show();

    // Подключаем сигналы
    connect(m_videoControls, &VideoControls::playPauseClicked, this, &PreviewPanel::onPlayPauseClicked);
    connect(m_videoControls, &VideoControls::stopClicked, this, &PreviewPanel::onStopClicked);
    connect(m_videoControls, &VideoControls::positionChanged, this, [this](qint64 pos) { if (m_mediaPlayer) m_mediaPlayer->setPosition(pos); });
    connect(m_videoControls, &VideoControls::volumeChanged, this, [this](int vol) { if (m_audioOutput) m_audioOutput->setVolume(vol / 100.0); });
    connect(m_videoControls, &VideoControls::fullscreenClicked, this, &PreviewPanel::onFullscreenClicked);

    // Медиа плеер
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_audioOutput->setVolume(0.7);

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PreviewPanel::onMediaStatusChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &PreviewPanel::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &PreviewPanel::onDurationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &PreviewPanel::onPlaybackStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error) { qDebug() << "Ошибка плеера:" << error; });

    m_videoControls->setVolume(70);
    m_videoControls->setLoading(true);
    playerLayout->addStretch();
}

void PreviewPanel::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if (m_videoControls) {
        m_videoControls->setPlaying(state == QMediaPlayer::PlayingState);
    }
}

void PreviewPanel::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        m_isVideoReady = true;
        if (m_videoStack) m_videoStack->setCurrentIndex(1);
        if (m_videoControls) {
            m_videoControls->setLoading(false);
            m_videoControls->showControls();
            m_videoControls->raise();
            m_videoControls->update();
        }
        // Запускаем через таймер, чтобы дать время на инициализацию
        m_loadTimer->start(500);
        qDebug() << "Видео загружено";
    }

    if (status == QMediaPlayer::InvalidMedia) {
        qDebug() << "Ошибка: не удалось загрузить видео";
        if (m_videoControls) {
            m_videoControls->setLoading(false);
        }
    }
}

void PreviewPanel::onPositionChanged(qint64 pos) { if (m_videoControls) m_videoControls->setPosition(pos); }
void PreviewPanel::onDurationChanged(qint64 dur) { if (m_videoControls) m_videoControls->setDuration(dur); }

void PreviewPanel::onPlayPauseClicked()
{
    if (m_mediaPlayer) {
        if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
            m_mediaPlayer->pause();
        } else {
            m_mediaPlayer->play();
        }
    }
}

void PreviewPanel::onStopClicked()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_isVideoReady = false;
        if (m_videoControls) {
            m_videoControls->setPosition(0);
            m_videoControls->setPlaying(false);
            m_videoControls->setLoading(true);
        }
        if (m_videoStack) m_videoStack->setCurrentIndex(0);
    }
}

void PreviewPanel::onFullscreenClicked() { toggleFullscreen(); }

void PreviewPanel::toggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    if (m_videoControls) {
        m_videoControls->setFullscreen(m_isFullscreen);
        // Принудительно показываем контролы в полноэкранном режиме
        if (m_isFullscreen) {
            m_videoControls->showControls();
            m_videoControls->raise();
        }
    }
    if (m_isFullscreen) {
        showFullScreen();
    } else {
        showNormal();
        resize(1024, 700);
    }
    // Обновляем позицию контролов после изменения размера
    QTimer::singleShot(100, this, [this]() {
        if (m_videoControls && m_videoContainer) {
            m_videoControls->setGeometry(0, 0, m_videoContainer->width(), m_videoContainer->height());
            m_videoControls->raise();
            m_videoControls->update();
        }
    });
}

void PreviewPanel::showPlayer(const QString &videoPath, const QString &caption, const QString &description, const QString &previewPath)
{
    Q_UNUSED(description)
    m_playerTitle->setText(caption);
    m_isVideoReady = false;

    if (m_videoStack) m_videoStack->setCurrentIndex(0);
    if (m_videoControls) {
        m_videoControls->setLoading(true);
        m_videoControls->setPlaying(false);
        m_videoControls->showControls();
        m_videoControls->raise();
        m_videoControls->update();
    }

    if (!previewPath.isEmpty()) {
        QPixmap preview(previewPath);
        if (!preview.isNull()) {
            m_playerPreview->setPixmap(preview.scaled(m_playerPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            m_playerPreview->setText("Загрузка видео...");
            m_playerPreview->setStyleSheet("color: #666; font-size: 18px;");
        }
    }

    QString fullPath = videoPath;
    if (!QFile::exists(fullPath)) {
        QStringList paths;
        paths << QCoreApplication::applicationDirPath() + "/videos/" + QFileInfo(videoPath).fileName();
        paths << QDir::currentPath() + "/videos/" + QFileInfo(videoPath).fileName();
        paths << QDir::homePath() + "/videos/" + QFileInfo(videoPath).fileName();
        for (const QString &p : paths) { if (QFile::exists(p)) { fullPath = p; break; } }
    }

    qDebug() << "Загружаем:" << fullPath;
    m_mediaPlayer->setSource(QUrl::fromLocalFile(fullPath));
    // Не запускаем сразу, ждем загрузки

    m_mainStack->setCurrentWidget(m_playerView);
}

void PreviewPanel::showStacks()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_isVideoReady = false;
        m_loadTimer->stop();
    }
    if (m_isFullscreen) toggleFullscreen();
    if (m_videoStack) m_videoStack->setCurrentIndex(0);
    m_mainStack->setCurrentWidget(m_stacksView);
}

void PreviewPanel::onBackClicked() { showStacks(); }

void PreviewPanel::onImageClicked(int index, const QString &videoPath, const QString &caption, const QString &description)
{
    QString previewPath = (index < m_imagePaths.size()) ? m_imagePaths[index] : QString();
    showPlayer(videoPath, caption, description, previewPath);
}
