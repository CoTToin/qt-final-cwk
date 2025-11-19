//
//
//

#include "the_player.h"
#include <QDebug>

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
    case QMediaPlayer::State::StoppedState:
        play(); // starting playing again...
        break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    // 保存当前播放速率
    double currentRate = playbackRate;

    setMedia(*button->url);

    // 恢复播放速率
    setPlaybackRate(currentRate);

    play();
}

// 设置播放速率
void ThePlayer::setPlaybackRate(double rate) {
    // 限制速率在合理范围内 (0.25x - 4.0x)
    rate = qBound(0.25, rate, 4.0);

    playbackRate = rate;
    QMediaPlayer::setPlaybackRate(rate);

    qDebug() << "Playback rate set to:" << playbackRate << "x";
}

// 增加播放速度
void ThePlayer::increaseSpeed() {
    // 常见的倍速梯度：0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0
    double newRate = playbackRate + 0.25;
    setPlaybackRate(newRate);
}

// 降低播放速度
void ThePlayer::decreaseSpeed() {
    double newRate = playbackRate - 0.25;
    setPlaybackRate(newRate);
}

// 重置为正常速度
void ThePlayer::resetSpeed() {
    setPlaybackRate(1.0);
}

// 获取当前播放速度
double ThePlayer::getCurrentSpeed() const {
    return playbackRate;
}

// 播放/暂停切换
void ThePlayer::togglePlayPause() {
    if (state() == QMediaPlayer::PlayingState) {
        pause();
        qDebug() << "Video paused";
    } else {
        play();
        qDebug() << "Video playing";
    }
}
