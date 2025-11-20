#include "the_player.h"
#include <QDebug>

ThePlayer::ThePlayer(QObject *parent)
    : QMediaPlayer(parent)
    , playbackRate(1.0)
{
    // 音量
    setVolume(70);

    // 视频结束自动重新播放
    connect(this, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(playStateChanged(QMediaPlayer::State)));

    // 每秒随机换一个缩略图
    mTimer = new QTimer(this);
    mTimer->setInterval(1000);
    mTimer->start();
    connect(mTimer, SIGNAL(timeout()), this, SLOT(shuffle()));
}

void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i)
{
    buttons = b;
    infos = i;
    if (!buttons->empty()) {
        jumpTo(buttons->at(0)->info);
    }
}

void ThePlayer::shuffle()
{
    if (infos->empty() || buttons->empty()) return;
    TheButtonInfo* i = &infos->at(rand() % infos->size());
    buttons->at(updateCount++ % buttons->size())->init(i);
}

void ThePlayer::playStateChanged(QMediaPlayer::State ms)
{
    if (ms == QMediaPlayer::StoppedState) {
        play(); // 自动循环播放
    }
}

void ThePlayer::jumpTo(TheButtonInfo* button)
{
    if (!button || !button->url) return;

    double currentRate = playbackRate;
    setMedia(QMediaContent(*button->url));
    setPlaybackRate(currentRate);
    play();
}

// 倍速功能
void ThePlayer::setPlaybackRate(double rate)
{
    rate = qBound(0.25, rate, 4.0);
    playbackRate = rate;
    QMediaPlayer::setPlaybackRate(rate);
    qDebug() << "Playback rate set to:" << playbackRate << "x";
}

void ThePlayer::increaseSpeed()
{
    double newRate = playbackRate + 0.25;
    if (newRate > 4.0) newRate = 4.0;
    setPlaybackRate(newRate);
}

void ThePlayer::decreaseSpeed()
{
    double newRate = playbackRate - 0.25;
    if (newRate < 0.25) newRate = 0.25;
    setPlaybackRate(newRate);
}

void ThePlayer::resetSpeed()
{
    setPlaybackRate(1.0);
}

double ThePlayer::getCurrentSpeed() const
{
    return playbackRate;
}

void ThePlayer::togglePlayPause()
{
    if (state() == QMediaPlayer::PlayingState) {
        pause();
        qDebug() << "Video paused";
    } else {
        play();
        qDebug() << "Video playing";
    }
}
