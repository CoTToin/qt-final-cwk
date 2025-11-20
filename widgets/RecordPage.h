// widgets/RecordPage.h
#pragma once
#include <QWidget>
#include <QCamera>
#include <QMediaRecorder>

class RecordPage : public QWidget {
    Q_OBJECT
public:
    explicit RecordPage(QWidget *parent = nullptr);

signals:
    void videoRecorded();   // 通知MainPage刷新

private:
    QCamera *backCamera = nullptr;
    QMediaRecorder *recorder = nullptr;
    void startRecording();
};
