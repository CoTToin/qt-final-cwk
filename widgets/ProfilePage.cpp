// widgets/ProfilePage.cpp   ← 最终简化版（取消高对比切换，固定深色主题）
#include "ProfilePage.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

ProfilePage::ProfilePage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 40, 20, 20);

    // 标题
    QLabel *title = new QLabel(tr("My Profile"));
    title->setStyleSheet("font-size: 36px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);

    // 用户名
    QLabel *nameLabel = new QLabel(tr("Username"));
    nameLabel->setStyleSheet("font-size: 18px; color: #AAAAAA;");
    QLineEdit *nameEdit = new QLineEdit("Xiaoming");
    nameEdit->setStyleSheet("font-size: 20px; padding: 12px; border-radius: 12px; background: #222; color: white;");
    nameEdit->setAccessibleName("usernameEdit");

    // 统计（模拟）
    QLabel *stats = new QLabel(tr("Videos posted: 12\nFriends: 48"));
    stats->setStyleSheet("font-size: 18px; color: #888; background: #111; padding: 20px; border-radius: 12px;");
    stats->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addSpacing(30);
    layout->addWidget(stats);
    layout->addStretch();
}
