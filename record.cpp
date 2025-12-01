// record.cpp
#include "record.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox> // 可选，如果需要其他弹窗

Record::Record(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#000000;");
    auto* mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);
    mainLay->addStretch(1);
    // 中间标题区（保持不变）
    auto* frame = new QFrame;
    frame->setFixedSize(380, 480);
    frame->setStyleSheet("background:#555; border-radius:30px; border:6px solid #333;");
    auto* text = new QLabel("Confidently Show Yourself");
    text->setStyleSheet("color:#888; font-size:24px; font-weight:bold;");
    text->setAlignment(Qt::AlignCenter);
    auto* frameLay = new QVBoxLayout(frame);
    frameLay->setContentsMargins(0, 0, 0, 0);
    frameLay->setSpacing(0);
    frameLay->addWidget(text, 0, Qt::AlignCenter);
    mainLay->addWidget(frame, 0, Qt::AlignCenter);
    mainLay->addStretch(1);
    // 新增：消息标签（初始隐藏）
    messageLabel = new QLabel(this);
    messageLabel->setText("");
    messageLabel->setStyleSheet("color:#ff4488; font-size:20px; font-weight:bold;");
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->hide();
    mainLay->addWidget(messageLabel);
    // 底部功能按钮栏
    auto* bar = new QWidget;
    bar->setFixedHeight(120);
    auto* barLay = new QHBoxLayout(bar);
    barLay->setContentsMargins(40,0,40,0);
    barLay->setSpacing(20); // 减小间距，使按钮更紧凑
    // 闪光灯按钮（⚡ 图标，优化样式）
    auto* flash = new QPushButton;
    flash->setFixedSize(70, 70);
    flash->setText(QChar(0x26A1)); // 闪电图标⚡（U+26A1）
    flash->setStyleSheet(R"(
        QPushButton {
            font-family: "Segoe UI Symbol", "Arial Unicode MS", "Noto Sans Symbols", sans-serif;
            font-size: 55px;
            background: transparent;
            color: #ccc;
            border: none;
            text-shadow: 0 0 8px rgba(255, 255, 255, 0.2);
        }
        QPushButton:hover {
            color: #fff;
            text-shadow: 0 0 12px rgba(255, 255, 255, 0.5);
            transform: scale(1.1);
            transition: all 0.2s ease;
        }
        QPushButton:pressed {
            color: #eee;
            text-shadow: 0 0 6px rgba(255, 255, 255, 0.3);
            transform: scale(0.95);
        }
    )");
    connect(flash, &QPushButton::clicked, this, &Record::flashClicked);
    // 录制按钮（保持不变）
    auto* record = new QPushButton;
    record->setFixedSize(100,100);
    record->setStyleSheet("background:#ff4488; border-radius:50px; border:8px solid #ff88bb;");
    auto* inner = new QLabel;
    inner->setFixedSize(46,46);
    inner->setStyleSheet("background:white; border-radius:6px;");
    inner->setAlignment(Qt::AlignCenter);
    auto* recordLay = new QVBoxLayout(record);
    recordLay->addWidget(inner, 0, Qt::AlignCenter);
    connect(record, &QPushButton::clicked, this, &Record::onRecordClicked); // 修改：连接到新槽函数
    // 发布按钮（保持不变，可按需优化）
    auto* post = new QPushButton("Post");
    post->setFixedSize(70,70);
    post->setStyleSheet(R"(
        QPushButton {
            font-size: 36px;
            background: transparent;
            color: #ccc;
            border: none;
            transition: all 0.2s ease;
        }
        QPushButton:hover {
            color: #fff;
            transform: scale(1.1);
        }
        QPushButton:pressed {
            color: #eee;
            transform: scale(0.95);
        }
    )");
    connect(post, &QPushButton::clicked, this, &Record::postClicked);
    // 按钮栏布局：添加外侧拉伸，中间按钮间距小，使闪电和Post更靠近录制按钮
    barLay->addStretch(1);
    barLay->addWidget(flash);
    barLay->addSpacing(40); // 闪电到录制的额外小间距
    barLay->addWidget(record, 0, Qt::AlignCenter);
    barLay->addSpacing(40); // 录制到Post的额外小间距
    barLay->addWidget(post);
    barLay->addStretch(1);
    mainLay->addWidget(bar);
}

// 新增：录制按钮槽函数实现
void Record::onRecordClicked()
{
    messageLabel->setText("Filming will start in 10s.");
    messageLabel->show(); // 显示消息
    emit recordClicked(); // 可选：同时发出信号，供外部处理
}
