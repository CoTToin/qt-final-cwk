// core/ThePlayer.cpp
#include "ThePlayer.h"

ThePlayer::ThePlayer(QObject *parent) : QMediaPlayer(parent) {
    setVolume(70);
    connect(this, &QMediaPlayer::positionChanged, this, &ThePlayer::positionChanged);
    connect(this, &QMediaPlayer::durationChanged, this, &ThePlayer::durationChanged);
}

void ThePlayer::jumpTo(TheButtonInfo* info) {
    setMedia(*info->url);
    play();
}
