#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QMediaPlayer>
#include <vector>
#include <QTimer>
#include "the_button.h"

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos = nullptr;
    std::vector<TheButton*>* buttons = nullptr;
    QTimer* mTimer = nullptr;
    long updateCount = 0;
    double playbackRate = 1.0;

public:
    explicit ThePlayer(QObject *parent = nullptr);

    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

    // 倍速相关
    void setPlaybackRate(double rate);
    void increaseSpeed();
    void decreaseSpeed();
    void resetSpeed();
    double getCurrentSpeed() const;
    void togglePlayPause();

private slots:
    void shuffle();
    void playStateChanged(QMediaPlayer::State ms);

public slots:
    void jumpTo(TheButtonInfo* button);
};

#endif // CW2_THE_PLAYER_H
