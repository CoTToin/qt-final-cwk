// the_player.cpp —— 完美修复：倍速真实生效 + 界面实时更新！（删除 shuffle，优化后端支持）
#include "the_player.h"
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <QDateTime>
#include <algorithm>
#include <cmath>

ThePlayer::ThePlayer(QObject *parent)
    : QMediaPlayer(parent), m_playbackRate(1.0) // 修改：使用 m_playbackRate
{
    setVolume(70);
    // 自动播放 + 循环
    connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);
    // 新增：媒体状态变化
    connect(this, &QMediaPlayer::mediaStatusChanged, this, &ThePlayer::mediaStatusChanged);
    // 删除 shuffle：缩略图固定不变
    // mTimer = new QTimer(this);
    // mTimer->start(1000);
    // connect(mTimer, &QTimer::timeout, this, &ThePlayer::shuffle);
    qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
}

void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i)
{
    buttons = b;
    infos = i;
}

void ThePlayer::playStateChanged(QMediaPlayer::State ms)
{
    if (ms == QMediaPlayer::StoppedState) {
        play(); // 自动循环播放
    }
}

void ThePlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        // 新增：媒体加载完成后重新应用当前倍速（修复切换视频后率丢失）
        qDebug() << "Media loaded, reapplying playback rate:" << m_playbackRate;
        QMediaPlayer::setPlaybackRate(m_playbackRate); // 修改：使用 m_playbackRate
        emit playbackRateChanged(m_playbackRate); // 修改：使用 m_playbackRate
    } else if (status == QMediaPlayer::InvalidMedia) {
        qDebug() << "Invalid media, playback rate may not apply";
    }
}

void ThePlayer::jumpTo(TheButtonInfo* button)
{
    if (!button || !button->url) return;
    // 保持当前倍速切换视频
    double currentRate = m_playbackRate; // 修改：使用 m_playbackRate
    setMedia(QMediaContent(*button->url));
    // 不立即设置率，等 mediaStatusChanged 处理
    play();
}

size_t ThePlayer::findCurrentSpeedIndex() const
{
    for (size_t i = 0; i < speedRates.size(); ++i) {
        if (std::abs(speedRates[i] - m_playbackRate) < 0.01) { // 修改：使用 m_playbackRate
            return i;
        }
    }
    return std::distance(speedRates.begin(), std::lower_bound(speedRates.begin(), speedRates.end(), m_playbackRate)); // 修改：使用 m_playbackRate
}

void ThePlayer::setPlaybackRate(double rate)
{
    // 找到最近的可用倍速
    double nearest = 1.0;
    double minDiff = std::abs(rate - 1.0);
    for (double r : speedRates) {
        double diff = std::abs(r - rate);
        if (diff < minDiff) {
            minDiff = diff;
            nearest = r;
        }
    }
    m_playbackRate = nearest; // 修改：使用 m_playbackRate
    // 关键：真正设置播放速率（Qt 后端可能不支持某些格式，qDebug 调试）
    QMediaPlayer::setPlaybackRate(nearest);
    qDebug() << "Set playback rate to:" << nearest << "(actual backend rate:" << QMediaPlayer::playbackRate() << ")"; // 修改：使用 QMediaPlayer::playbackRate()
    // 如果后端不支持，playbackRate() 可能仍为1.0
    if (std::abs(QMediaPlayer::playbackRate() - nearest) > 0.01) { // 修改：使用 QMediaPlayer::playbackRate()
        qDebug() << "Warning: Backend may not support this playback rate!";
    }
    // 发送信号，让 Home 界面刷新倍速标签
    emit playbackRateChanged(nearest);
}

void ThePlayer::increaseSpeed()
{
    size_t idx = findCurrentSpeedIndex();
    if (idx < speedRates.size() - 1) {
        setPlaybackRate(speedRates[idx + 1]);
    }
    // 如果已在最高速，保持不变
}

void ThePlayer::decreaseSpeed()
{
    size_t idx = findCurrentSpeedIndex();
    if (idx > 0) {
        setPlaybackRate(speedRates[idx - 1]);
    }
    // 如果已在最低速，保持不变
}

void ThePlayer::resetSpeed()
{
    setPlaybackRate(1.0);
}

double ThePlayer::getCurrentSpeed() const
{
    return m_playbackRate; // 修改：返回内部目标速率（UI 显示用）
    // 如果想返回后端实际速率，可改为 return QMediaPlayer::playbackRate();
}

void ThePlayer::togglePlayPause()
{
    if (state() == QMediaPlayer::PlayingState)
        pause();
    else
        play();
}
