// widgets/MainPage.cpp   ← 完整终极版（包含所有函数定义，已修复undefined reference）
#include "MainPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QRandomGenerator>
#include <QDebug>

MainPage::MainPage(QWidget *parent) : QWidget(parent)
{
    VideoManager::instance();
    setupUI();

    connect(VideoManager::instance(), &VideoManager::videoAdded, this, &MainPage::refreshVideoGrid);
}

MainPage::~MainPage() = default;

void MainPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumHeight(520);
    videoWidget->setStyleSheet("background:#000;border-radius:16px;");
    videoWidget->setAccessibleName("mainVideoWidget");

    player = new ThePlayer(this);
    player->setVideoOutput(videoWidget);

    mainLayout->addWidget(videoWidget);

    setupPlayerControls();
    mainLayout->addLayout(playerControlsLayout);

    setupCommentsAndRealMoji();
    mainLayout->addLayout(commentsSectionLayout);

    gridContainer = new QWidget(this);
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(16);
    createVideoGrid();

    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(gridContainer);
    scroll->setStyleSheet("QScrollArea {background:transparent; border:none;}");
    mainLayout->addWidget(scroll, 1);

}

void MainPage::setupPlayerControls()
{
    playerControlsLayout = new QHBoxLayout;

    QPushButton *playBtn = new QPushButton(tr("Play / Pause"));
    playBtn->setAccessibleName("playPauseButton");
    playBtn->setFixedHeight(48);
    connect(playBtn, &QPushButton::clicked, player, [this](){
        player->state() == QMediaPlayer::PlayingState ? player->pause() : player->play();
    });

    seekSlider = new QSlider(Qt::Horizontal);
    seekSlider->setRange(0, 0);
    seekSlider->setAccessibleName("seekSlider");

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeSlider->setMaximumWidth(140);
    volumeSlider->setAccessibleName("volumeSlider");
    connect(volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);

    speedCombo = new QComboBox;
    speedCombo->addItems({"0.5×", "0.75×", "1.0×", "1.5×", "2.0×"});
    speedCombo->setCurrentIndex(2);
    speedCombo->setAccessibleName("playbackSpeedCombo");
    connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int i){
        double rates[] = {0.5, 0.75, 1.0, 1.5, 2.0};
        player->setPlaybackRate(rates[i]);
    });

    QPushButton *fullscreenBtn = new QPushButton(tr("Full Screen"));
    fullscreenBtn->setAccessibleName("fullscreenButton");
    connect(fullscreenBtn, &QPushButton::clicked, videoWidget, &QVideoWidget::setFullScreen);

    playerControlsLayout->addWidget(playBtn);
    playerControlsLayout->addWidget(seekSlider);
    playerControlsLayout->addWidget(new QLabel(tr("Vol:")));
    playerControlsLayout->addWidget(volumeSlider);
    playerControlsLayout->addWidget(speedCombo);
    playerControlsLayout->addWidget(fullscreenBtn);

    connect(player, &ThePlayer::positionChanged, this, &MainPage::updateSeekSlider);
    connect(player, &ThePlayer::durationChanged, this, &MainPage::updateDuration);
    connect(seekSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
}

void MainPage::setupCommentsAndRealMoji()
{
    commentsSectionLayout = new QVBoxLayout;

    QLabel *title = new QLabel(tr("Comments & Reactions"));
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin: 16px 0 8px 0;");
    commentsSectionLayout->addWidget(title);

    commentsList = new QListWidget;
    commentsList->setAccessibleName("commentsList");
    commentsList->setStyleSheet("border-radius: 12px;");
    commentsList->addItem(tr("This is so real! 🔥"));
    commentsList->addItem(tr("Love the dual camera! 😭"));
    commentsSectionLayout->addWidget(commentsList);

    QHBoxLayout *inputBox = new QHBoxLayout;
    QLineEdit *input = new QLineEdit;
    input->setPlaceholderText(tr("Write a comment..."));
    input->setAccessibleName("commentInput");
    QPushButton *send = new QPushButton(tr("Send"));
    send->setFixedWidth(80);
    inputBox->addWidget(input);
    inputBox->addWidget(send);
    commentsSectionLayout->addLayout(inputBox);

    connect(send, &QPushButton::clicked, this, [this, input]() {
        if (!input->text().isEmpty()) {
            commentsList->addItem("Me: " + input->text());
            input->clear();
        }
    });

    QHBoxLayout *mojiBar = new QHBoxLayout;
    mojiBar->addStretch();
    QStringList emojis = {"❤️", "🔥", "😂", "😢", "😍", "🥳", "👀"};
    for (const QString& e : emojis) {
        QPushButton *b = new QPushButton(e);
        b->setFixedSize(56, 56);
        b->setStyleSheet("font-size: 32px; border-radius: 28px;");
        connect(b, &QPushButton::clicked, this, [this, e]() { addRealMojiReaction(e); });
        mojiBar->addWidget(b);
    }
    mojiBar->addStretch();
    commentsSectionLayout->addLayout(mojiBar);
}

void MainPage::createVideoGrid()
{
    buttons.clear();
    QLayoutItem *child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int columns = width() < 900 ? 2 : 4;
    const auto& videos = VideoManager::instance()->getAllVideos();

    int row = 0, col = 0;
    for (const auto& info : videos) {
        TheButton *btn = new TheButton(gridContainer);
        btn->init(const_cast<TheButtonInfo*>(&info));
        btn->setIconSize(QSize(180, 320));
        btn->setStyleSheet("QPushButton { border-radius: 16px; background: #111; }");
        btn->setAccessibleName("videoThumbnailButton");

        connect(btn, &TheButton::jumpTo, this, &MainPage::onVideoClicked);

        gridLayout->addWidget(btn, row, col++);
        buttons.append(btn);

        if (col >= columns) { col = 0; row++; }
    }
}

void MainPage::onVideoClicked(TheButtonInfo* info)
{
    player->jumpTo(info);

    commentsList->clear(); //每次点新视频清空评论

    commentsList->addItem(tr("Now playing: ") + info->url->fileName());
    commentsList->scrollToBottom();
}

void MainPage::addRealMojiReaction(const QString& emoji)
{
    // ---------- 1. 弹幕式横向飘过视频（超级酷！） ----------
    QLabel *barrage = new QLabel(emoji, videoWidget);  // 关键：父对象是 videoWidget！
    barrage->setStyleSheet("font-size: 80px; background: transparent; color: white;");
    barrage->resize(100, 100);
    barrage->setAlignment(Qt::AlignCenter);

    // 随机垂直位置（在视频区域内）
    int randomY = QRandomGenerator::global()->bounded(50, videoWidget->height() - 150);
    barrage->move(videoWidget->width(), randomY);  // 从右侧出现
    barrage->show();
    barrage->raise();  // 确保在最上层

    // 横向动画：从右到左飘过
    QPropertyAnimation *anim = new QPropertyAnimation(barrage, "pos");
    anim->setDuration(4000 + QRandomGenerator::global()->bounded(2000)); // 4~6秒
    anim->setStartValue(QPoint(videoWidget->width(), randomY));
    anim->setEndValue(QPoint(-150, randomY));
    anim->setEasingCurve(QEasingCurve::Linear);

    // 动画结束自动删除
    connect(anim, &QPropertyAnimation::finished, barrage, &QObject::deleteLater);
    anim->start();

    // ---------- 2. 同时在评论区添加一条反应 ----------
    commentsList->addItem(tr("Me reacted: ") + emoji);
    commentsList->scrollToBottom();  // 自动滚到最新
}

void MainPage::updateSeekSlider(qint64 position)
{
    if (!seekSlider->isSliderDown())
        seekSlider->setValue(position);
}

void MainPage::updateDuration(qint64 duration)
{
    seekSlider->setRange(0, duration);
}

void MainPage::refreshVideoGrid()
{
    createVideoGrid();
}
