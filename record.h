// record.h
#ifndef RECORD_H
#define RECORD_H
#include <QWidget>
#include <QLabel> // 新增：为消息标签添加头文件
class Record : public QWidget {
    Q_OBJECT
public:
    explicit Record(QWidget *parent = nullptr);
signals:
    void recordClicked();
    void postClicked();
    void flashClicked();
private slots:
    void onRecordClicked(); // 新增：录制按钮槽函数
private:
    QLabel *messageLabel = nullptr; // 新增：消息标签成员
};
#endif // RECORD_H
