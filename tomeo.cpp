// tomeo.cpp —— 极简版，只负责窗口和切换
#undef connect
#undef emit

#include <QApplication>
#include <QtWidgets>
#include <QStackedWidget>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include "home.h"
#include "record.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QString videoPath = argc >= 2 ? QString::fromLocal8Bit(argv[1])
                                  : QCoreApplication::applicationDirPath() + "/videos";

    auto videos = Home::loadVideosFromPath(videoPath);
    if (videos.empty()) {
        QMessageBox::critical(nullptr, "MomentV", "没找到视频！\n请检查 videos 文件夹");
        return -1;
    }

    QStackedWidget *stack = new QStackedWidget;

    Home *homePage = new Home;
    homePage->setVideos(videos);
    stack->addWidget(homePage);
    stack->addWidget(new Record);
    stack->addWidget(new QLabel("Heart"));
    stack->addWidget(new QLabel("Profile"));

    auto *navBar = new QWidget;
    navBar->setFixedHeight(90);
    navBar->setStyleSheet("background:rgba(0,0,0,240);");
    auto *navLay = new QHBoxLayout(navBar);
    navLay->setContentsMargins(30,0,30,0);
    navLay->setSpacing(40);

    QVector<QToolButton*> navBtns;
    QStringList labels = {"Home", "Camera", "Heart", "Person"};

    for (int i = 0; i < 4; ++i) {
        auto *btn = new QToolButton;
        btn->setFixedSize(70,70);
        btn->setText(labels[i]);
        btn->setStyleSheet("color:#888; font-size:36px; background:transparent; border:none;");
        btn->setCheckable(true);
        QObject::connect(btn, &QToolButton::clicked, [=]() {
            stack->setCurrentIndex(i);
            for (auto *b : navBtns) b->setChecked(false);
            btn->setChecked(true);
        });
        navLay->addWidget(btn);
        navBtns.append(btn);
    }
    navBtns[0]->setChecked(true);
    navBtns[0]->setStyleSheet("color:white; font-size:36px; background:transparent; border:none;");

    QWidget window;
    auto *mainLay = new QVBoxLayout(&window);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);
    mainLay->addWidget(stack, 1);
    mainLay->addWidget(navBar);

    window.setWindowTitle("MomentV");
    window.resize(755, 924);
    window.setMinimumSize(320, 568);
    window.setFixedWidth(755);

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect g = screen->geometry();
        window.move((g.width() - window.width()) / 2, 0);
    }

    window.show();
    return app.exec();
}
