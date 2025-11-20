// widgets/MainPage.h
#pragma once
#include <QWidget>
#include <QVideoWidget>
#include <QGridLayout>
#include <QSlider>
#include <QListWidget>
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include "../core/ThePlayer.h"
#include "../core/VideoManager.h"
#include "the_button.h"

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();

    void refreshVideoGrid();        // 被RecordPage调用刷新视频列表

private:
    ThePlayer *player;
    QVideoWidget *videoWidget;
    QGridLayout *gridLayout;
    QWidget *gridContainer;
    QListWidget *commentsList;

    // 新增：控制栏和评论区布局（修复 undeclared identifier）
    QHBoxLayout *playerControlsLayout = nullptr;
    QVBoxLayout *commentsSectionLayout = nullptr;

    // 控制栏组件
    QSlider *seekSlider = nullptr;
    QSlider *volumeSlider = nullptr;
    QComboBox *speedCombo = nullptr;

    QVector<TheButton*> buttons;

    void setupUI();
    void setupPlayerControls();
    void setupCommentsAndRealMoji();
    void createVideoGrid();
    void onVideoClicked(TheButtonInfo* info);
    void addRealMojiReaction(const QString& emoji);

private slots:
    void updateSeekSlider(qint64 position);
    void updateDuration(qint64 duration);
};
