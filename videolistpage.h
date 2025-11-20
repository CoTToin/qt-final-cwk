// videolistpage.h
#ifndef VIDEOLISTPAGE_H
#define VIDEOLISTPAGE_H

#include <QWidget>
#include "the_player.h"

class VideoListPage : public QWidget
{
    Q_OBJECT

public:
    explicit VideoListPage(ThePlayer* player, QWidget *parent = nullptr);

private:
    void setupUI();
    std::vector<TheButtonInfo> loadVideos(const QString& path);

    ThePlayer* m_player;
};

#endif // VIDEOLISTPAGE_H
