// videolistpage.cpp
#include "videolistpage.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QScrollArea>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QFile>
#include <QImageReader>
#include <QObject>

VideoListPage::VideoListPage(ThePlayer* player, QWidget *parent)
    : QWidget(parent)
    , m_player(player)  // 保存指针
{
    setupUI();
}

void VideoListPage::setupUI()
{
    setStyleSheet("background:black;");

    QVBoxLayout* mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);

    QWidget* listWidget = new QWidget;
    QVBoxLayout* listLay = new QVBoxLayout(listWidget);
    listLay->setSpacing(15);
    listLay->setContentsMargins(15,15,15,15);

    std::vector<TheButtonInfo> videos = loadVideos(QCoreApplication::applicationDirPath() + "/videos");
    std::vector<TheButton*> buttons;

    for (size_t i = 0; i < videos.size(); ++i) {
        TheButton* btn = new TheButton(listWidget);
        QObject::connect(btn, SIGNAL(jumpTo(TheButtonInfo*)), m_player, SLOT(jumpTo(TheButtonInfo*)));
        btn->init(&videos[i]);
        buttons.push_back(btn);
        listLay->addWidget(btn);
    }
    listLay->addStretch();
    m_player->setContent(&buttons, &videos);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(listWidget);
    scroll->setStyleSheet("background:black; border:none;");

    mainLay->addWidget(scroll, 1);
}

std::vector<TheButtonInfo> VideoListPage::loadVideos(const QString& folderPath)
{
    std::vector<TheButtonInfo> out;
    QDir dir(folderPath);
    if (!dir.exists()) return out;

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
