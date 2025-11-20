//
// ______
// /_ __/___ ____ ___ ___ ____
// / / / __ \/ __ `__ \/ _ \/ __ \
// / / / /_/ / / / / / / __/ /_/ /
// /_/ \____/_/ /_/ /_/\___/\____/
// video for sports enthusiasts...
//
//
#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QScrollArea>
#include <QStackedWidget>
#include <QLabel>
#include <QObject>
#include <QFile>
#include <QImageReader>
#include "the_player.h"
#include "the_button.h"

// 读取视频
std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);
    while (it.hasNext()) {
        QString f = it.next();
        if (f.contains(".")) {
#if defined(_WIN32)
            if (f.contains(".wmv"))
#else
            if (f.contains(".mp4") || f.contains(".MOV"))
#endif
            {
                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) {
                    QImageReader reader(thumb);
                    QImage sprite = reader.read();
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite));
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f));
                        out.push_back(TheButtonInfo(url, ico));
                    }
                }
            }
        }
    }
    return out;
}

// 创建美化后的控制按钮
QWidget* createControlButtons(ThePlayer* player) {
    QWidget* control = new QWidget;
    control->setStyleSheet("background:rgba(0,0,0,180); border-radius:12px; margin:10px;");

    QHBoxLayout* layout = new QHBoxLayout(control);
    layout->setSpacing(30);
    layout->setContentsMargins(20, 15, 20, 15);

    // 播放/暂停大图标
    QPushButton* playBtn = new QPushButton("▶️");
    playBtn->setFixedSize(70, 70);
    playBtn->setStyleSheet("font-size:36px; background:transparent; color:white; border:none;");
    QObject::connect(playBtn, &QPushButton::clicked, player, [player, playBtn]() {
        if (player->state() == QMediaPlayer::PlayingState) {
            player->pause();
            playBtn->setText("▶️");
        } else {
            player->play();
            playBtn->setText("⏸");
        }
    });

    // 倍速控制（–  1×  +）
    QPushButton* slow = new QPushButton("–");
    QPushButton* normal = new QPushButton("1×");
    QPushButton* fast = new QPushButton("+");

    slow->setFixedSize(60, 60); normal->setFixedSize(60, 60); fast->setFixedSize(60, 60);
    slow->setStyleSheet("font-size:28px; background:transparent; color:white; border:none;");
    normal->setStyleSheet("font-size:24px; background:transparent; color:white; border:none;");
    fast->setStyleSheet("font-size:28px; background:transparent; color:white; border:none;");

    QObject::connect(slow, SIGNAL(clicked()), player, SLOT(decreaseSpeed()));
    QObject::connect(normal, SIGNAL(clicked()), player, SLOT(resetSpeed()));
    QObject::connect(fast, SIGNAL(clicked()), player, SLOT(increaseSpeed()));

    layout->addStretch();
    layout->addWidget(slow);
    layout->addWidget(normal);
    layout->addWidget(fast);
    layout->addStretch();
    layout->addWidget(playBtn, 0, Qt::AlignRight);

    return control;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::vector<TheButtonInfo> videos;
    if (argc == 2)
        videos = getInfoIn(argv[1]);
    if (videos.empty()) {
        QMessageBox::information(nullptr, "Tomeo", "No videos found!");
        return -1;
    }


    QVideoWidget* videoWidget = new QVideoWidget;
    videoWidget->setStyleSheet("background:black; border-radius:16px;");
    ThePlayer* player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    QWidget* fixedPlayArea = new QWidget;
    fixedPlayArea->setStyleSheet("background:black;");
    QVBoxLayout* fixedLay = new QVBoxLayout(fixedPlayArea);
    fixedLay->setContentsMargins(0, 20, 0, 20);  // 上方留空间居中
    fixedLay->addStretch();
    fixedLay->addWidget(videoWidget, 0, Qt::AlignCenter);  // 严格水平居中
    fixedLay->addSpacing(20);
    fixedLay->addWidget(createControlButtons(player), 0, Qt::AlignCenter);  // 控制按钮居中
    fixedLay->addStretch();

    // 缩略图列表
    QWidget* buttonContainer = new QWidget;
    QVBoxLayout* btnLay = new QVBoxLayout(buttonContainer);
    btnLay->setSpacing(12);
    btnLay->setContentsMargins(10,10,10,10);
    std::vector<TheButton*> buttons;

    for (int i = 0; i < qMin(10, (int)videos.size()); ++i) {
        TheButton* b = new TheButton(buttonContainer);
        QObject::connect(b, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        b->init(&videos[i]);
        buttons.push_back(b);
        btnLay->addWidget(b);
    }
    btnLay->addStretch();
    player->setContent(&buttons, &videos);

    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(buttonContainer);
    scroll->setStyleSheet("background:black; border:none;");

    // 可切换内容区
    QWidget* pageHome = new QWidget; pageHome->setStyleSheet("background:black;");
    QWidget* pageList = new QWidget;
    QVBoxLayout* listLay = new QVBoxLayout(pageList);
    listLay->addWidget(scroll, 1);

    QWidget* pageProfile = new QWidget; pageProfile->setStyleSheet("background:black;");
    QVBoxLayout* profLay = new QVBoxLayout(pageProfile);
    QLabel* lbl = new QLabel("个人中心");
    lbl->setStyleSheet("color:white; font-size:28px;");
    lbl->setAlignment(Qt::AlignCenter);
    profLay->addWidget(lbl, 1, Qt::AlignCenter);

    QStackedWidget* contentStack = new QStackedWidget;
    contentStack->addWidget(pageHome);
    contentStack->addWidget(pageList);
    contentStack->addWidget(pageProfile);

    //底部导航
    QHBoxLayout* nav = new QHBoxLayout;
    nav->setSpacing(50);
    nav->setContentsMargins(20,10,20,10);

    QPushButton* tab1 = new QPushButton("🏠");
    QPushButton* tab2 = new QPushButton("📹");
    QPushButton* tab3 = new QPushButton("👤");
    tab1->setFixedSize(70,70); tab2->setFixedSize(70,70); tab3->setFixedSize(70,70);
    tab1->setStyleSheet("font-size:32px; background:transparent; color:#888; border:none;");
    tab2->setStyleSheet("font-size:32px; background:transparent; color:#888; border:none;");
    tab3->setStyleSheet("font-size:32px; background:transparent; color:#888; border:none;");

    QObject::connect(tab1, SIGNAL(clicked()), contentStack, SLOT(setCurrentIndex(0)));
    QObject::connect(tab2, SIGNAL(clicked()), contentStack, SLOT(setCurrentIndex(1)));
    QObject::connect(tab3, SIGNAL(clicked()), contentStack, SLOT(setCurrentIndex(2)));

    nav->addWidget(tab1);
    nav->addStretch();
    nav->addWidget(tab2);
    nav->addStretch();
    nav->addWidget(tab3);

    QWidget* navBar = new QWidget;
    navBar->setStyleSheet("background:rgba(0,0,0,220);");
    navBar->setLayout(nav);

    // 主窗口
    QWidget window;
    window.setStyleSheet("background:black;");
    QVBoxLayout* main = new QVBoxLayout(&window);
    videoWidget->setFixedSize(360, 420);
    main->setContentsMargins(0,0,0,0);
    main->setSpacing(0);

    main->addWidget(fixedPlayArea, 1);      // 视频+控制永远在上层
    main->addWidget(contentStack, 1);       // 内容区
    main->addWidget(navBar);                // 底部导航

    window.setWindowTitle("Tomeo");
    window.setFixedSize(450,700);
    window.show();

    return app.exec();
}
