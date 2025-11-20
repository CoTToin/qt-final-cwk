// core/VideoManager.h
#pragma once
#include <QObject>
#include <QVector>
#include "the_button.h"

class VideoManager : public QObject {
    Q_OBJECT
public:
    static VideoManager* instance();
    const QVector<TheButtonInfo>& getAllVideos() const { return videos; }
    void addVideo(const QString& filePath);

signals:
    void videoAdded();

private:
    static VideoManager* m_instance;
    QVector<TheButtonInfo> videos;
    VideoManager() = default;
};
