// main.cpp
#include <QApplication>
#include <QTranslator>
#include <QInputDialog>
#include <QMainWindow>
#include <QTabWidget>
#include <QIcon>
#include "widgets/MainPage.h"
#include "widgets/RecordPage.h"
#include "widgets/ProfilePage.h"
#include "widgets/FriendsPage.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    QStringList langs {"English", "中文"};
    if (QInputDialog::getItem(nullptr, "Language", "Select:", langs, 0, false) == "中文") {
        translator.load(":/translations/momentv_zh.qm");
        a.installTranslator(&translator);
    }

    QMainWindow window;
    QTabWidget *tabs = new QTabWidget;
    tabs->setTabPosition(QTabWidget::South);
    tabs->setIconSize(QSize(36,36));

    tabs->addTab(new MainPage,   QIcon(":/icons/home.svg"),    "");
    tabs->addTab(new RecordPage, QIcon(":/icons/record.svg"), "");
    tabs->addTab(new FriendsPage, QIcon(":/icons/friends.svg"), "");
    tabs->addTab(new ProfilePage, QIcon(":/icons/profile.svg"), "");

    window.setCentralWidget(tabs);
    window.setWindowTitle("MomentV");
    window.resize(920, 1000);
    window.show();

    return a.exec();
}
