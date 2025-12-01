// the_player.h
#ifndef THE_PLAYER_H
#define THE_PLAYER_H
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
    // long updateCount = 0; // 删除：不再需要
    double m_playbackRate = 1.0; // 修改：重命名避免冲突
    std::vector<double> speedRates = {0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0};
public:
    explicit ThePlayer(QObject *parent = nullptr);
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    void setPlaybackRate(double rate);
    void increaseSpeed();
    void decreaseSpeed();
    void resetSpeed();
    double getCurrentSpeed() const;
    void togglePlayPause();
private slots:
    void playStateChanged(QMediaPlayer::State ms);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status); // 新增：确保加载后设置率
    // void shuffle(); // 删除：不再需要
    size_t findCurrentSpeedIndex() const;
public slots:
    void jumpTo(TheButtonInfo* button);
signals:
    void playbackRateChanged(double rate);
};
#endif // THE_PLAYER_H
