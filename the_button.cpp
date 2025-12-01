// the_button.cpp
#include "the_button.h"

TheButton::TheButton(QWidget *parent)
    : QPushButton(parent)
{
    setIconSize(QSize(360, 240));
    setFixedSize(360, 240);
    QObject::connect(this, &QPushButton::clicked, this, &TheButton::onJumpToClicked);
}

void TheButton::init(TheButtonInfo* i)
{
    info = i;
    if (info && info->icon)
        setIcon(*info->icon);
}

void TheButton::onJumpToClicked()
{
    if (info)
        emit jumpTo(info);
}
