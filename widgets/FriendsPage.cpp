// widgets/FriendsPage.cpp
#include "FriendsPage.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

FriendsPage::FriendsPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);

    // 搜索框
    QLineEdit *search = new QLineEdit;
    search->setPlaceholderText(tr("Search friends..."));
    search->setFixedHeight(44);
    layout->addWidget(search);

    // 好友列表
    QListWidget *list = new QListWidget;
    list->setSpacing(4);  // 列表项间距
    QStringList friends = {tr("Alice"), tr("Bob"), tr("Charlie")};

    for (const QString &name : friends) {
        QListWidgetItem *item = new QListWidgetItem(name, list);
        item->setSizeHint(QSize(0, 60));  // 移动端列表项高度
    }
    layout->addWidget(list);

    // 添加好友按钮固定在底部
    QPushButton *add = new QPushButton(tr("+ Add Friend"));
    add->setFixedHeight(50);
    add->setStyleSheet("font-size:18px; background:#4CAF50; color:white;");
    layout->addWidget(add);
}
