#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>

class TheButtonInfo {
public:
    QUrl* url;     // 视频文件路径
    QIcon* icon;   // 缩略图图标

    TheButtonInfo(QUrl* u, QIcon* i) : url(u), icon(i) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info = nullptr;  // 当前按钮关联的视频信息

    explicit TheButton(QWidget *parent = nullptr) : QPushButton(parent) {
        setIconSize(QSize(200, 110));
        setFixedSize(200, 110);  // 可选：固定按钮大小，更整齐

        // 点击按钮时自动发出 jumpTo 信号
        connect(this, &QPushButton::clicked, this, &TheButton::onJumpToClicked);
    }

    // 初始化按钮：设置图标和关联信息
    void init(TheButtonInfo* i) {
        info = i;
        if (info && info->icon) {
            setIcon(*info->icon);
        }
    }

private slots:
    void onJumpToClicked() {
        if (info) {
            emit jumpTo(info);  // 发出信号，让 player 切换视频
        }
    }

signals:
    void jumpTo(TheButtonInfo*);  // 点击按钮时发出，让播放器跳转
};

#endif // CW2_THE_BUTTON_H
