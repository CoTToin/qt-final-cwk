// home.cpp
#include "home.h"
#include "the_player.h"
#include <QtWidgets>
#include <QtMultimediaWidgets>
#include <QTimer>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWindow>
#include <QGuiApplication> // 用于 windowHandle() 等
// 实现静态加载函数（支持21-30个视频，只过滤 mp4/mov/wmv）
std::vector<TheButtonInfo> Home::loadVideosFromPath(const QString& path)
{
    std::vector<TheButtonInfo> videos;
    QString p = path.trimmed();
    if (p.startsWith('"') && p.endsWith('"')) p = p.mid(1, p.size()-2);
    QDir dir(p);
    if (!dir.exists()) return videos;
    // 只找视频文件，效率高，只支持 mp4/mov/wmv
    QStringList filters = {"*.mp4", "*.mov", "*.wmv"};
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Name);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    int count = 0;
    int maxVideos = 30; // 支持最多30个
    for (const QFileInfo& fileInfo : fileList) {
        if (count >= maxVideos) break;
        QString baseName = fileInfo.baseName(); // 获取文件名（不带扩展名）
        QString thumb = dir.absoluteFilePath(baseName + ".png");
        if (QFile::exists(thumb)) {
            QImage img(thumb);
            if (!img.isNull()) {
                videos.push_back(TheButtonInfo{
                    new QUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath())),
                    new QIcon(QPixmap::fromImage(img.scaled(360,240,Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation)))
                });
                count++;
            }
        }
    }
    return videos;
}
Home::Home(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#0a0a0a;");
}
void Home::setVideos(const std::vector<TheButtonInfo>& v)
{
    videos = v;
    likes.resize(v.size(), false); // 初始化点赞状态
    likeBtns.resize(v.size(), nullptr); // 初始化按钮指针
    setupUI();
}
void Home::setupUI()
{
    videoWidget = new QVideoWidget(this);
    videoWidget->setStyleSheet("background:black; border-radius:18px;");
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    player = new ThePlayer(this);
    player->setVideoOutput(videoWidget);
    speedLabel = new QLabel("1.00×", this);
    speedLabel->setStyleSheet("color:white; font-size:26px; font-weight:bold;");
    speedLabel->setAlignment(Qt::AlignCenter);
    // 进度条初始化
    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setRange(0, 1000);
    progressSlider->setValue(0);
    // 音量条初始化（0-100）
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70); // 默认 70
    // 新增：音量显示标签
    volumeLabel = new QLabel("70", this);
    volumeLabel->setStyleSheet("color:white; font-size:18px;"); // 小白色字体
    volumeLabel->setFixedHeight(40);
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [=]() {
        if (player && player->duration() > 0) {
            qint64 pos = player->position();
            qint64 dur = player->duration();
            int value = static_cast<int>((pos * 1000) / dur);
            if (!isFullScreen) {
                progressSlider->blockSignals(true); // 防止循环触发
                progressSlider->setValue(value);
                progressSlider->blockSignals(false);
            } else {
                // 全屏进度条更新（专用）
                if (fullScreenProgressSlider) {
                    fullScreenProgressSlider->blockSignals(true);
                    fullScreenProgressSlider->setValue(value);
                    fullScreenProgressSlider->blockSignals(false);
                }
            }
            // 更新主标签（全屏时同步到全屏标签）
            double rate = player->getCurrentSpeed();
            speedLabel->setText(QString::number(rate, 'f', 2) + "×");
            if (fullScreenSpeedLabel) {
                fullScreenSpeedLabel->setText(QString::number(rate, 'f', 2) + "×");
            }
            // 更新音量标签（全屏同步）
            int vol = player->volume();
            volumeLabel->setText(QString::number(vol));
            if (fullScreenVolumeLabel) {
                fullScreenVolumeLabel->setText(QString::number(vol));
            }
        }
    });
    timer->start(100);
    auto *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);
    auto *playerArea = new QWidget;
    playerArea->setObjectName("playerArea"); // 用于退出时查找
    playerArea->setStyleSheet("background:#111;");
    auto *pLay = new QVBoxLayout(playerArea);
    pLay->setContentsMargins(15,15,15,0); // 无 bottom margin
    pLay->addWidget(videoWidget, 1);
    controlBarWidget = createControlBar();
    pLay->addWidget(controlBarWidget, 0, Qt::AlignCenter);
    // 视频列表
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:#0a0a0a; border:none;");
    auto *listWidget = new QWidget;
    auto *listLay = new QVBoxLayout(listWidget);
    listLay->setSpacing(18);
    listLay->setContentsMargins(15,10,15,20);
    buttons.clear();
    for (int i = 0; i < videos.size(); ++i) {
        auto *card = new QFrame;
        card->setStyleSheet("background:rgba(255,255,255,8); border-radius:18px; border:3px solid transparent;");
        card->setCursor(Qt::PointingHandCursor);
        auto *cardLay = new QVBoxLayout(card);
        cardLay->setContentsMargins(14,14,14,14);
        auto *header = new QHBoxLayout;
        auto *avatar = new QPushButton("Avatar", card);
        avatar->setFixedSize(48,48);
        avatar->setStyleSheet("font-size:26px;background:#555;color:white;border-radius:24px;");
        // 修改：根据视频数量创建对应用户（user_1 ~ user_N）
        int userId = i + 1;
        auto *name = new QLabel("user_" + QString::number(userId), card);
        name->setStyleSheet("color:white;font-size:18px;font-weight:bold;");
        header->addWidget(avatar);
        header->addWidget(name);
        header->addStretch();
        auto *btn = new TheButton(card);
        btn->init(&videos[i]);
        QObject::connect(btn, &TheButton::jumpTo, player, &ThePlayer::jumpTo);
        buttons.push_back(btn);
        cardLay->addLayout(header);
        cardLay->addWidget(btn);
        // 新增：点赞按钮、评论输入框和发送按钮一排
        auto *likeBtn = new QPushButton("♡", card);
        likeBtn->setFixedSize(40, 40);
        likeBtn->setStyleSheet("font-size:40px; color:white; background:transparent; border:none;");
        likeBtns[i] = likeBtn; // 存储按钮指针
        QObject::connect(likeBtn, &QPushButton::clicked, this, [=]() { onLikeClicked(i); });
        auto *commentInput = new QLineEdit(card);
        commentInput->setPlaceholderText("Add a comment...");
        commentInput->setFixedHeight(40);
        commentInput->setStyleSheet("QLineEdit { background:rgba(255,255,255,20); border:1px solid rgba(255,255,255,30); border-radius:12px; color:white; padding:8px; font-size:14px; }");
        auto *sendBtn = new QPushButton("Send", card);
        sendBtn->setFixedWidth(80);
        sendBtn->setFixedHeight(40);
        sendBtn->setStyleSheet("QPushButton { background:#ff4488; color:white; border:none; border-radius:12px; font-size:14px; font-weight:bold; } QPushButton:pressed { background:#cc3366; }");
        QObject::connect(sendBtn, &QPushButton::clicked, [=]() {
            QString text = commentInput->text().trimmed();
            if (!text.isEmpty()) {
                // TODO: 处理评论发送逻辑，例如存储或发送到服务器
                commentInput->clear(); // 清空输入框
            }
        });
        auto *interactionLay = new QHBoxLayout;
        interactionLay->setContentsMargins(0, 0, 0, 0);
        interactionLay->setSpacing(10);
        interactionLay->addWidget(likeBtn);
        interactionLay->addWidget(commentInput, 1);
        interactionLay->addWidget(sendBtn);
        cardLay->addLayout(interactionLay);
        listLay->addWidget(card);
    }
    listLay->addStretch();
    player->setContent(&buttons, &videos);
    scroll->setWidget(listWidget);
    mainLay->addWidget(playerArea, 5);
    mainLay->addWidget(scroll, 4);
    // 自动播放 user1 的视频（videos[0]）
    if (!videos.empty()) {
        player->jumpTo(&videos[0]);
    }
    // 新增：连接 ThePlayer 的倍速信号到 speedLabel 更新（全屏/非全屏通用）
    QObject::connect(player, &ThePlayer::playbackRateChanged, this, [=](double rate) {
        speedLabel->setText(QString::number(rate, 'f', 2) + "×");
        if (fullScreenSpeedLabel) {
            fullScreenSpeedLabel->setText(QString::number(rate, 'f', 2) + "×");
        }
    });
    // 新增：错误处理（调试倍速问题）—— Qt5.12 兼容
    QObject::connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
                     this, [this](QMediaPlayer::Error error) {
                         qDebug() << "Player error:" << error << this->player->errorString();
                     });
}
void Home::onLikeClicked(int videoIndex)
{
    likes[videoIndex] = !likes[videoIndex];
    QPushButton *btn = likeBtns[videoIndex];
    if (btn) {
        if (likes[videoIndex]) {
            btn->setText("♥");
            btn->setStyleSheet("font-size:40px; color:#ff4488; background:transparent; border:none;"); // 红心
        } else {
            btn->setText("♡");
            btn->setStyleSheet("font-size:40px; color:white; background:transparent; border:none;"); // 白心
        }
    }
}
QWidget* Home::createControlBar()
{
    auto *bar = new QWidget;
    bar->setStyleSheet("background:rgba(0,0,0,220); border-radius:18px; margin:8px;");
    bar->setFixedHeight(130); // 加高一点放进度条
    auto *mainLay = new QVBoxLayout(bar);
    mainLay->setContentsMargins(25,10,25,10);
    mainLay->setSpacing(12);
    // 进度条（美观粉色）
    progressSlider->setFixedHeight(8);
    progressSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            border-radius: 4px;
            height: 8px;
            background: rgba(255,255,255,60);
        }
        QSlider::handle:horizontal {
            background: white;
            width: 20px;
            height: 20px;
            margin: -6px 0;
            border-radius: 10px;
        }
        QSlider::sub-page:horizontal {
            background: #ff4488;
            border-radius: 4px;
        }
    )");
    // 拖动跳转
    QObject::connect(progressSlider, &QSlider::sliderMoved, this, [=](int value) {
        if (player && player->duration() > 0) {
            qint64 target = (player->duration() * value) / 1000;
            player->setPosition(target);
        }
    });
    // 播放按钮 + 倍速 + 音量条
    auto *btnLay = new QHBoxLayout;
    btnLay->setSpacing(20);
    auto *playBtn = new QPushButton;
    playBtn->setFixedSize(90,90);
    playBtn->setStyleSheet(R"(
        QPushButton {font-size:56px; background:transparent; color:white; border:none; border-radius:45px;
                     font-family:"Segoe UI Emoji","Apple Color Emoji";}
        QPushButton:pressed {background:rgba(255,255,255,30);}
    )");
    playBtn->setText("▶"); // 修改：初始播放图标
    QObject::connect(playBtn, &QPushButton::clicked, [=]() mutable {
        player->togglePlayPause();
        playBtn->setText(player->state() == QMediaPlayer::PlayingState ? "⏸" : "▶"); // 修改：切换图标
    });
    auto *speedBox = new QWidget;
    auto *sLay = new QHBoxLayout(speedBox);
    sLay->setSpacing(10);
    auto *slow = new QPushButton("−");
    slow->setFixedSize(40,40);
    slow->setStyleSheet("font-size:34px;color:white;background:transparent;border:none;");
    auto *fast = new QPushButton("+");
    fast->setFixedSize(40,40);
    fast->setStyleSheet("font-size:34px;color:white;background:transparent;border:none;");
    QObject::connect(slow, &QPushButton::clicked, player, &ThePlayer::decreaseSpeed);
    QObject::connect(fast, &QPushButton::clicked, player, &ThePlayer::increaseSpeed);
    sLay->addWidget(slow);
    sLay->addWidget(speedLabel);
    sLay->addWidget(fast);
    // 新增：音量条 + 显示标签（右侧小白色字体）
    auto *volumeBox = new QWidget;
    auto *volumeLay = new QHBoxLayout(volumeBox);
    volumeLay->setSpacing(5);
    volumeLay->setContentsMargins(0,0,0,0);
    volumeSlider->setFixedWidth(120);
    volumeSlider->setFixedHeight(40);
    volumeSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            border-radius: 4px;
            height: 6px;
            background: rgba(255,255,255,60);
        }
        QSlider::handle:horizontal {
            background: white;
            width: 16px;
            height: 16px;
            margin: -5px 0;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #ff4488;
            border-radius: 4px;
        }
    )");
    volumeLabel->setFixedWidth(40);
    volumeLabel->setAlignment(Qt::AlignCenter);
    QObject::connect(volumeSlider, &QSlider::valueChanged, this, [=](int value) {
        player->setVolume(value);
        volumeLabel->setText(QString::number(value));
        // 全屏同步
        if (fullScreenVolumeSlider) {
            fullScreenVolumeSlider->setValue(value);
        }
    });
    volumeLay->addWidget(volumeSlider);
    volumeLay->addWidget(volumeLabel);
    // 新增：全屏按钮（使用成员变量）
    fullScreenBtn = new QPushButton("FS", bar);
    fullScreenBtn->setFixedSize(40, 40);
    fullScreenBtn->setStyleSheet(R"(
        QPushButton {
            font-size: 16px; color: white; background: transparent; border: 2px solid white; border-radius: 20px;
            font-weight: bold;
        }
        QPushButton:pressed {
            background: rgba(255,255,255,30);
        }
    )");
    // 连接到槽函数
    QObject::connect(fullScreenBtn, &QPushButton::clicked, this, &Home::toggleFullScreen);
    btnLay->addStretch();
    btnLay->addWidget(playBtn);
    btnLay->addStretch();
    btnLay->addWidget(speedBox);
    btnLay->addWidget(volumeBox); // 音量条 + 标签在右侧
    btnLay->addWidget(fullScreenBtn); // 全屏按钮在音量右侧
    btnLay->addStretch();
    mainLay->addWidget(progressSlider);
    mainLay->addLayout(btnLay);
    return bar;
}
// 新增：切换全屏槽函数
void Home::toggleFullScreen()
{
    QWidget* topWin = window();
    if (!topWin) return;
    QWindow* wnd = topWin->windowHandle();
    if (!wnd) return;
    // 正确检查全屏状态：使用位运算检查标志
    if ((wnd->windowState() & Qt::WindowFullScreen) != 0) {
        exitFullScreen();
    } else {
        // 进入全屏：隐藏列表和控制栏，窗口全屏，videoWidget 自动扩展
        QScrollArea* scrollArea = findChild<QScrollArea*>();
        if (scrollArea) scrollArea->hide();
        if (controlBarWidget) controlBarWidget->hide();
        videoWidget->setStyleSheet("background:black;"); // 全屏移除圆角
        topWin->showFullScreen();
        fullScreenBtn->setText("Exit");
        // 创建全屏专用进度条
        if (!fullScreenProgressSlider) {
            fullScreenProgressSlider = new QSlider(Qt::Horizontal, topWin);
            fullScreenProgressSlider->setRange(0, 1000);
            fullScreenProgressSlider->setFixedHeight(8);
            fullScreenProgressSlider->setStyleSheet(R"(
                QSlider::groove:horizontal {
                    border-radius: 4px;
                    height: 8px;
                    background: rgba(255,255,255,60);
                }
                QSlider::handle:horizontal {
                    background: white;
                    width: 20px;
                    height: 20px;
                    margin: -6px 0;
                    border-radius: 10px;
                }
                QSlider::sub-page:horizontal {
                    background: #ff4488;
                    border-radius: 4px;
                }
            )");
            // 全屏进度条拖动连接
            QObject::connect(fullScreenProgressSlider, &QSlider::sliderMoved, this, [=](int value) {
                if (player && player->duration() > 0) {
                    qint64 target = (player->duration() * value) / 1000;
                    player->setPosition(target);
                }
            });
        }
        // 创建/显示覆盖层
        if (!fullScreenOverlay) {
            fullScreenOverlay = new QWidget(topWin);
            fullScreenOverlay->setStyleSheet("background:rgba(0,0,0,0);"); // 透明背景
            fullScreenOverlay->raise(); // 置顶
            auto *overlayLay = new QVBoxLayout(fullScreenOverlay);
            overlayLay->setContentsMargins(0,0,0,0);
            overlayLay->setSpacing(0);
            overlayLay->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
            // 全屏控制栏
            auto *overlayBar = new QWidget(fullScreenOverlay);
            overlayBar->setStyleSheet("background:rgba(0,0,0,220); border-radius:18px; margin:8px;");
            overlayBar->setFixedHeight(130);
            auto *oMainLay = new QVBoxLayout(overlayBar);
            oMainLay->setContentsMargins(25,10,25,10);
            oMainLay->setSpacing(12);
            // 使用全屏专用进度条
            oMainLay->addWidget(fullScreenProgressSlider);
            // 按钮布局：播放 + 倍速 + 音量 + 退出
            auto *oBtnLay = new QHBoxLayout;
            oBtnLay->setSpacing(20);
            oBtnLay->addStretch();
            // 播放按钮
            auto *oPlayBtn = new QPushButton(fullScreenOverlay);
            oPlayBtn->setFixedSize(90,90);
            oPlayBtn->setStyleSheet(R"(
                QPushButton {font-size:56px; background:transparent; color:white; border:none; border-radius:45px;}
                QPushButton:pressed {background:rgba(255,255,255,30);}
            )");
            oPlayBtn->setText("▶"); // 修改：初始播放图标
            QObject::connect(oPlayBtn, &QPushButton::clicked, [=]() mutable {
                player->togglePlayPause();
                oPlayBtn->setText(player->state() == QMediaPlayer::PlayingState ? "⏸" : "▶"); // 修改：切换图标
            });
            oBtnLay->addWidget(oPlayBtn);
            oBtnLay->addStretch();
            // 倍速盒（创建专用 label，避免复用）
            auto *oSpeedBox = new QWidget(fullScreenOverlay);
            auto *oSLay = new QHBoxLayout(oSpeedBox);
            oSLay->setSpacing(10);
            auto *oSlow = new QPushButton("−", fullScreenOverlay);
            oSlow->setFixedSize(40,40);
            oSlow->setStyleSheet("font-size:34px;color:white;background:transparent;border:none;");
            auto *oFast = new QPushButton("+", fullScreenOverlay);
            oFast->setFixedSize(40,40);
            oFast->setStyleSheet("font-size:34px;color:white;background:transparent;border:none;");
            QObject::connect(oSlow, &QPushButton::clicked, player, &ThePlayer::decreaseSpeed);
            QObject::connect(oFast, &QPushButton::clicked, player, &ThePlayer::increaseSpeed);
            fullScreenSpeedLabel = new QLabel("1.00×", fullScreenOverlay); // 新增：专用 label
            fullScreenSpeedLabel->setStyleSheet("color:white; font-size:26px; font-weight:bold;");
            fullScreenSpeedLabel->setAlignment(Qt::AlignCenter);
            oSLay->addWidget(oSlow);
            oSLay->addWidget(fullScreenSpeedLabel); // 使用专用
            oSLay->addWidget(oFast);
            oBtnLay->addWidget(oSpeedBox);
            // 音量盒（创建专用 slider 和 label，避免复用）
            auto *oVolumeBox = new QWidget(fullScreenOverlay);
            auto *oVolumeLay = new QHBoxLayout(oVolumeBox);
            oVolumeLay->setSpacing(5);
            oVolumeLay->setContentsMargins(0,0,0,0);
            fullScreenVolumeSlider = new QSlider(Qt::Horizontal, fullScreenOverlay); // 新增：专用 slider
            fullScreenVolumeSlider->setRange(0, 100);
            fullScreenVolumeSlider->setValue(player->volume());
            fullScreenVolumeSlider->setFixedWidth(120);
            fullScreenVolumeSlider->setFixedHeight(40);
            fullScreenVolumeSlider->setStyleSheet(R"(
                QSlider::groove:horizontal {
                    border-radius: 4px;
                    height: 6px;
                    background: rgba(255,255,255,60);
                }
                QSlider::handle:horizontal {
                    background: white;
                    width: 16px;
                    height: 16px;
                    margin: -5px 0;
                    border-radius: 8px;
                }
                QSlider::sub-page:horizontal {
                    background: #ff4488;
                    border-radius: 4px;
                }
            )");
            QObject::connect(fullScreenVolumeSlider, &QSlider::valueChanged, this, [=](int value) {
                player->setVolume(value);
                if (fullScreenVolumeLabel) {
                    fullScreenVolumeLabel->setText(QString::number(value));
                }
                // 主音量同步
                if (volumeSlider) {
                    volumeSlider->setValue(value);
                }
            });
            fullScreenVolumeLabel = new QLabel(QString::number(player->volume()), fullScreenOverlay); // 新增：专用 label
            fullScreenVolumeLabel->setStyleSheet("color:white; font-size:18px;");
            fullScreenVolumeLabel->setFixedWidth(40);
            fullScreenVolumeLabel->setFixedHeight(40);
            fullScreenVolumeLabel->setAlignment(Qt::AlignCenter);
            oVolumeLay->addWidget(fullScreenVolumeSlider);
            oVolumeLay->addWidget(fullScreenVolumeLabel);
            oBtnLay->addWidget(oVolumeBox);
            // 退出按钮
            exitFullScreenBtn = new QPushButton("Exit FS", fullScreenOverlay);
            exitFullScreenBtn->setFixedSize(80, 40);
            exitFullScreenBtn->setStyleSheet(R"(
                QPushButton {font-size:14px; color:white; background:#ff4488; border:none; border-radius:20px;}
                QPushButton:pressed {background:#cc3366;}
            )");
            QObject::connect(exitFullScreenBtn, &QPushButton::clicked, this, &Home::exitFullScreen);
            oBtnLay->addWidget(exitFullScreenBtn);
            oBtnLay->addStretch();
            oMainLay->addLayout(oBtnLay);
            overlayLay->addWidget(overlayBar);
        }
        fullScreenOverlay->show();
        fullScreenOverlay->setGeometry(0, topWin->height() - 150, topWin->width(), 150); // 底部
        isFullScreen = true;
        // 主定时器已处理全屏更新，无需额外定时器
    }
}
// 新增：退出全屏槽函数
void Home::exitFullScreen()
{
    QWidget* topWin = window();
    if (!topWin) return;
    topWin->showNormal();
    fullScreenBtn->setText("FS");
    QScrollArea* scrollArea = findChild<QScrollArea*>();
    if (scrollArea) scrollArea->show();
    if (controlBarWidget) controlBarWidget->show();
    videoWidget->setStyleSheet("background:black; border-radius:18px;"); // 恢复圆角
    // 布局自动调整 videoWidget 大小，继续播放
    if (fullScreenOverlay) {
        fullScreenOverlay->hide();
    }
    isFullScreen = false;
}
