//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>

class TheButtonInfo {

public:
    QUrl* url; // video file to play
    QIcon* icon; // icon to display

    TheButtonInfo ( QUrl* url, QIcon* icon) : url (url), icon (icon) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

    TheButton(QWidget *parent) :  QPushButton(parent) {
        setIconSize(QSize(160, 90));  // 移动端图标尺寸
        setFixedSize(180, 120);       // 固定按钮尺寸
        setStyleSheet(R"(
        QPushButton {
            border: 2px solid #e0e0e0;
            border-radius: 12px;
            padding: 8px;
        }
        QPushButton:pressed {
            background: #f0f0f0;
        }
    )");
        connect(this, SIGNAL(released()), this, SLOT(clicked()));
    }

    void init(TheButtonInfo* i);

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);

};

#endif //CW2_THE_BUTTON_H
