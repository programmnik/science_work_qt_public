#include "previewpanel.h"
#include <QScrollArea>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <QObject>
#include <QEvent>

PreviewStack::PreviewStack(const QString &title, const QStringList &imagePaths,
        const QStringList &captions, const QStringList &videoPaths, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_imagePaths(imagePaths)
    , m_captions(captions)
    , m_videoPaths(videoPaths)
    , m_currentIndex(0)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(15);

    m_titleLabel = new QLabel(m_title);
    m_titleLabel->setStyleSheet(R"(
        QLabel {
            color: #e3e3e3;
            font-size: 20px;
            font-weight: 600;
            padding: 10px 0px;
            border-bottom: 2px solid #0d7377;
        }
    )");
    mainLayout->addWidget(m_titleLabel);

    QHBoxLayout *imageContainer = new QHBoxLayout();
    imageContainer->setContentsMargins(0, 20, 0, 20);
    imageContainer->setSpacing(20);

    m_prevButton = new QPushButton("◀");
    m_prevButton->setFixedSize(50, 200);
    m_prevButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(13, 115, 119, 0.5);
            border-radius: 10px;
            color: white;
            font-size: 24px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(13, 115, 119, 0.8);
        }
    )");
    connect(m_prevButton, &QPushButton::clicked, this, &PreviewStack::onPrevClicked);
    imageContainer->addWidget(m_prevButton);

    m_imageLabel = new QLabel();
    m_imageLabel->setFixedSize(600, 400);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet(R"(
        QLabel {
            border: 2px solid #0d7377;
            border-radius: 20px;
            background-color: #1a1a2e;
        }
    )");
    m_imageLabel->setCursor(Qt::PointingHandCursor);
    m_imageLabel->installEventFilter(this);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(15);
    shadowEffect->setOffset(3, 3);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    m_imageLabel->setGraphicsEffect(shadowEffect);

    imageContainer->addWidget(m_imageLabel);

    m_nextButton = new QPushButton("▶");
    m_nextButton->setFixedSize(50, 200);
    m_nextButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(13, 115, 119, 0.5);
            border-radius: 10px;
            color: white;
            font-size: 24px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(13, 115, 119, 0.8);
        }
    )");
    connect(m_nextButton, &QPushButton::clicked, this, &PreviewStack::onNextClicked);
    imageContainer->addWidget(m_nextButton);

    mainLayout->addLayout(imageContainer);

    m_captionLabel = new QLabel();
    m_captionLabel->setAlignment(Qt::AlignCenter);
    m_captionLabel->setStyleSheet(R"(
        QLabel {
            color: #c0c0c0;
            font-size: 16px;
            padding: 10px;
            background-color: rgba(0, 0, 0, 0.5);
            border-radius: 10px;
        }
    )");
    mainLayout->addWidget(m_captionLabel);

    updateImage(0);
}

void PreviewStack::updateImage(int index){
    if(index >= 0 && index < m_imagePaths.size()){
        m_currentIndex = index;
        QPixmap pixmap(m_imagePaths[index]);
        if(!pixmap.isNull()){
            QPixmap scaled = pixmap.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QPixmap result(scaled.size());
            result.fill(Qt::transparent);

            QPainter painter(&result);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawPixmap(0, 0, scaled);

            QLinearGradient gradient(0, scaled.height() * 0.33, 0, scaled.height());
            gradient.setColorAt(0, QColor(0, 0, 0, 0));
            gradient.setColorAt(1, QColor(0, 0, 0, 230));
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.fillRect(scaled.rect(), gradient);

            painter.end();
            m_imageLabel->setPixmap(result);
        } else {
            m_imageLabel->setText("Изображение не найдено");
        }
        m_captionLabel->setText(m_captions[index]);
    }

    m_prevButton->setEnabled(m_currentIndex > 0);
    m_nextButton->setEnabled(m_currentIndex < m_imagePaths.size() - 1);

    QString style = m_prevButton->isEnabled()
                        ? "background-color: rgba(13, 115, 119, 0.5); "
                          "border-radius: 10px; color: white; font-size: 24px;"
                        : "background-color: rgba(80, 80, 80, 0.3); "
                          "border-radius: 10px; color: gray; font-size: 24px;";
    m_prevButton->setStyleSheet(style);

    style = m_nextButton->isEnabled()
                ? "background-color: rgba(13, 115, 119, 0.5); "
                  "border-radius: 10px; color: white; font-size: 24px;"
                : "background-color: rgba(80, 80, 80, 0.3); "
                  "border-radius: 10px; color: gray; font-size: 24px;";

    m_nextButton->setStyleSheet(style);
}

void PreviewStack::onPrevClicked(){
    if(m_currentIndex > 0){
        updateImage(m_currentIndex - 1);
    }
}

void PreviewStack::onNextClicked(){
    if(m_currentIndex < m_imagePaths.size() - 1){
        updateImage(m_currentIndex + 1);
    }
}

void PreviewStack::onImageClicked()
{
    emit imageClicked(m_currentIndex, m_videoPaths[m_currentIndex], m_captions[m_currentIndex]);
}

bool PreviewStack::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_imageLabel && event->type() == QEvent::MouseButtonPress){
        onImageClicked();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

PreviewPanel::PreviewPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

PreviewPanel::~PreviewPanel()
{
    if(m_mediaPlayer){
        m_mediaPlayer->stop();
    }
}

void PreviewPanel::setupUI(){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 20);

    m_stacksView = new QWidget(this);
    QVBoxLayout *stacksViewLayout = new QVBoxLayout(m_stacksView);
    stacksViewLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("Выберите превью которое вы хотите посмотреть:");
    title->setStyleSheet("background-color: #292857;"
                         "padding: 20px;"
                         "border: 2px solid #171b2b;"
                         "border-radius: 20px;"
                         "color: #e3e3e3;"
                         "font-size: 26px;"
                         "font-weight: 600;"
                         "margin-bottom: 20px;"
                         );
    stacksViewLayout->addWidget(title);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollBar:vertical {
            background-color: #2b2b2b;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background-color: #0d7377;
            border-radius: 4px;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

    QWidget *stacksContainer = new QWidget();
    QVBoxLayout *stacksLayout = new QVBoxLayout(stacksContainer);
    stacksLayout->setContentsMargins(0, 0, 0, 0);
    stacksLayout->setSpacing(30);

    setupStacks(stacksLayout);
    stacksLayout->addStretch();

    scrollArea->setWidget(stacksContainer);
    stacksViewLayout->addWidget(scrollArea);

    mainLayout->addWidget(m_stacksView);

    setupPlayer();
    mainLayout->addWidget(m_playerView);
    m_playerView->setVisible(false);
}

void PreviewPanel::setupStacks(QVBoxLayout *stacksLayout){
    QStringList imagePaths;
    QStringList caption;
    QStringList videoPaths;

    imagePaths << ":images/preview1.png" << ":/images/preview2.png";
    caption << "Стереографическая проекция" << "Иллюстрация бесконечности";
    videoPaths << ":/videos/preview1.mp4" << ":/videos/preview2.mp4";

    PreviewStack *mainStack = new PreviewStack("Основные превью", imagePaths, caption, videoPaths);
    connect(mainStack, &PreviewStack::imageClicked, this, &PreviewPanel::onImageClicked);
    stacksLayout->addWidget(mainStack);
}

void PreviewPanel::setupPlayer()
{
    m_playerView = new QWidget(this);
    QVBoxLayout *playerLayout = new QVBoxLayout(m_playerView);
    playerLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *topBar = new QHBoxLayout();
    m_backButton = new QPushButton("← Назад к выбору");
    m_backButton->setStyleSheet(R"(
        QPushButton {
            background-color: #0d7377;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            color: white;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0f8b8f;
        }
    )");
    connect(m_backButton, &QPushButton::clicked, this, &PreviewPanel::showStacks);
    topBar->addWidget(m_backButton);
    topBar->addStretch();
    playerLayout->addLayout(topBar);

    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setStyleSheet("border: 2px solid #0d7377; border-radius: 20px;");
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    playerLayout->addWidget(m_videoWidget, 1);

    m_playerCaption = new QLabel();
    m_playerCaption->setAlignment(Qt::AlignCenter);
    m_playerCaption->setStyleSheet(R"(
        QLabel {
            color: #c0c0c0;
            font-size: 16px;
            padding: 10px;
            background-color: rgba(0, 0, 0, 0.5);
            border-radius: 10px;
            margin-top: 10px;
        }
    )");
    playerLayout->addWidget(m_playerCaption);
}

void PreviewPanel::showPlayer(const QString &videoPath, const QString &caption)
{
    m_stacksView->setVisible(false);
    m_playerView->setVisible(true);

    m_playerCaption->setText(caption);
    m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    m_mediaPlayer->play();
}

void PreviewPanel::showStacks()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
    m_playerView->setVisible(false);
    m_stacksView->setVisible(true);
}

void PreviewPanel::onBackClicked()
{

}

void PreviewPanel::onImageClicked(int index, const QString &videoPath, const QString &caption)
{
    showPlayer(videoPath, caption);
}
