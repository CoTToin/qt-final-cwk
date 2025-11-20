// core/ThePlayer.h
#pragma once
#include <QMediaPlayer>
#include "the_button.h"

class ThePlayer : public QMediaPlayer {
    Q_OBJECT
public:
    explicit ThePlayer(QObject *parent = nullptr);
    void jumpTo(TheButtonInfo* info);

signals:
    void positionChanged(qint64);
    void durationChanged(qint64);
};
