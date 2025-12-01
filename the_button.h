// the_button.h
#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>

class TheButtonInfo {
public:
    QUrl* url;
    QIcon* icon;
    TheButtonInfo(QUrl* u, QIcon* i) : url(u), icon(i) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info = nullptr;

    explicit TheButton(QWidget *parent = nullptr);  // 只声明！不要写 {}
    void init(TheButtonInfo* i);

private slots:
    void onJumpToClicked();

signals:
    void jumpTo(TheButtonInfo*);
};

#endif // CW2_THE_BUTTON_H
