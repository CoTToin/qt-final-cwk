// core/VideoManager.cpp
#include "VideoManager.h"
#include <QUrl>
#include <QIcon>

VideoManager* VideoManager::m_instance = nullptr;

VideoManager* VideoManager::instance() {
    if (!m_instance) m_instance = new VideoManager;
    return m_instance;
}

void VideoManager::addVideo(const QString& filePath) {
    QUrl* url = new QUrl(QUrl::fromLocalFile(filePath));
    QIcon* icon = new QIcon(":/placeholder.png");
    videos.append(TheButtonInfo(url, icon));
    emit videoAdded();
}
