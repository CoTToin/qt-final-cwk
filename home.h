// home.h
#ifndef HOME_H
#define HOME_H
#include <QWidget>
#include <QLabel>
#include <QVideoWidget>
#include <QSlider>
#include <vector>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include "the_button.h"
class ThePlayer;
class Home : public QWidget {
public:
    explicit Home(QWidget *parent = nullptr);
    ~Home() = default;
    // 静态函数，让外部直接调用加载视频
    static std::vector<TheButtonInfo> loadVideosFromPath(const QString& path);
    void setVideos(const std::vector<TheButtonInfo>& videos);
private slots:
    void toggleFullScreen(); // 新增：切换全屏槽
    void exitFullScreen(); // 新增：退出全屏槽
    void onLikeClicked(int videoIndex); // 新增：点赞槽
private:
    void setupUI();
    QWidget* createControlBar();
    ThePlayer *player = nullptr;
    QVideoWidget *videoWidget = nullptr;
    QLabel *speedLabel = nullptr;
    QSlider *progressSlider = nullptr;
    QSlider *volumeSlider = nullptr; // 音量条成员
    QLabel *volumeLabel = nullptr; // 新增：音量显示标签
    // 新增：全屏相关成员
    QPushButton *fullScreenBtn = nullptr;
    QWidget *fullScreenOverlay = nullptr;
    QPushButton *exitFullScreenBtn = nullptr;
    QSlider *fullScreenProgressSlider = nullptr; // 全屏专用进度条
    QWidget *controlBarWidget = nullptr; // 普通控制栏
    bool isFullScreen = false;
    std::vector<TheButtonInfo> videos;
    std::vector<TheButton*> buttons;
    // 新增：点赞状态和按钮指针
    std::vector<bool> likes; // 每个视频的点赞状态
    std::vector<QPushButton*> likeBtns; // 每个视频的点赞按钮指针
    // 新增：全屏专用标签（避免复用导致消失）
    QLabel *fullScreenSpeedLabel = nullptr;
    QLabel *fullScreenVolumeLabel = nullptr;
    QSlider *fullScreenVolumeSlider = nullptr;
};
#endif // HOME_H
