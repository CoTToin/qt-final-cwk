#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>

class ThePlayer : public QMediaPlayer {

    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;

    // 播放速率控制
    double playbackRate = 1.0;

public:
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(0); // be slightly less annoying
        connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)) );

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000); // 1000ms is one second between ...
        mTimer->start();
        connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method

        // 移除了所有快捷键代码
    }

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

    // 倍速播放相关方法（保留功能，通过界面按钮调用）
    void setPlaybackRate(double rate);
    void increaseSpeed();      // 增加播放速度
    void decreaseSpeed();      // 降低播放速度
    void resetSpeed();         // 重置为正常速度
    double getCurrentSpeed() const;
    void togglePlayPause();    // 播放/暂停切换

private slots:

    // change the image and video for one button every one second
    void shuffle();

    void playStateChanged (QMediaPlayer::State ms);

public slots:

    // start playing this ButtonInfo
    void jumpTo (TheButtonInfo* button);
};

#endif //CW2_THE_PLAYER_H
