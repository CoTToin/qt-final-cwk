//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

// 添加的头文件（用于页面切换和工具按钮）
#include <QStackedWidget>
#include <QLabel>
#include <QToolButton>
#include <QObject>

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);


    // create the four buttons
    for ( int i = 0; i < 4; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // 添加的代码开始：包装原有内容到页面1，并创建堆叠窗口
    // 创建页面1（原有视频界面）
    QWidget *page1 = new QWidget();
    QVBoxLayout *page1Layout = new QVBoxLayout();
    page1Layout->addWidget(videoWidget);
    page1Layout->addWidget(buttonWidget);
    page1->setLayout(page1Layout);

    // 创建页面2（占位）
    QWidget *page2 = new QWidget();
    QVBoxLayout *page2Layout = new QVBoxLayout();
    QLabel *label2 = new QLabel("Page 2 Content Here");
    label2->setAlignment(Qt::AlignCenter);
    page2Layout->addWidget(label2);
    page2->setLayout(page2Layout);

    // 创建页面3（占位）
    QWidget *page3 = new QWidget();
    QVBoxLayout *page3Layout = new QVBoxLayout();
    QLabel *label3 = new QLabel("Page 3 Content Here");
    label3->setAlignment(Qt::AlignCenter);
    page3Layout->addWidget(label3);
    page3->setLayout(page3Layout);

    // 创建页面4（占位）
    QWidget *page4 = new QWidget();
    QVBoxLayout *page4Layout = new QVBoxLayout();
    QLabel *label4 = new QLabel("Page 4 Content Here");
    label4->setAlignment(Qt::AlignCenter);
    page4Layout->addWidget(label4);
    page4->setLayout(page4Layout);

    // 创建堆叠窗口
    QStackedWidget *stack = new QStackedWidget();
    stack->addWidget(page1);
    stack->addWidget(page2);
    stack->addWidget(page3);
    stack->addWidget(page4);

    // 添加堆叠到主布局
    top->addWidget(stack);

    // 创建底部菜单栏
    QWidget *bottomBar = new QWidget();
    bottomBar->setFixedHeight(80);  // 增大高度
    bottomBar->setMinimumWidth(750);  // 强制最小宽度更宽
    bottomBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // 宽度随窗口扩展
    bottomBar->setStyleSheet("background-color: white; border-radius: 20px;");

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(20, 10, 20, 10);  // 增大边距，拉宽视觉
    bottomLayout->setSpacing(80);  // 增大按钮间距，使整体更宽
    bottomBar->setLayout(bottomLayout);

    // 添加左侧伸展，使按钮居中分布
    bottomLayout->addStretch(1);

    // 按钮列表，用于管理选中状态
    std::vector<QToolButton*> navButtons;

    // 创建4个按钮
    for (int i = 0; i < 4; ++i) {
        QToolButton *btn = new QToolButton();
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setIcon(QIcon(QString(":/mima/%1.png").arg(i + 1)));
        btn->setText(QString("page%1").arg(i + 1));
        btn->setIconSize(QSize(32, 32));  // 增大图标
        btn->setStyleSheet("QToolButton { background: transparent; color: #FFC0CB; font-size: 20px; font-weight: bold; }");  // 稍增大字体
        btn->setFixedSize(120, 60);  // 增大按钮尺寸

        bottomLayout->addWidget(btn);
        navButtons.push_back(btn);
    }

    for (int i = 0; i < 4; ++i) {
        QToolButton *btn = navButtons[i];
        // 连接点击事件：切换页面并更新选中样式
        QObject::connect(btn, &QToolButton::clicked, [stack, navButtons, i, btn]() {
            stack->setCurrentIndex(i);
            // 重置所有按钮样式
            for (auto b : navButtons) {
                b->setStyleSheet("QToolButton { background: transparent; color: #FFC0CB; font-size: 20px; font-weight: bold; }");
            }
            // 选中当前按钮：黑色背景，白色文本，圆角
            btn->setStyleSheet("QToolButton { background: #808080; color: white; border-radius: 20px; font-size: 20px; font-weight: bold; }");
        });
    }

    bottomLayout->addStretch(1);

    // 添加菜单栏到主布局底部，并水平居中
    top->addWidget(bottomBar, 0, Qt::AlignBottom | Qt::AlignHCenter);

    // 初始选中page1
    navButtons[0]->click();

    // 添加的代码结束

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
