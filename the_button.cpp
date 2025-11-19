//
//
//

#include "the_button.h"

void TheButton::init(TheButtonInfo* i) {
    setIcon( *(i->icon) );
    info =  i;

    // ==================== 修改部分开始 ====================
    // 清除所有现有连接
    disconnect();

    // 连接左键点击信号到跳转功能
    connect(this, &QPushButton::clicked, this, &TheButton::onJumpToClicked);
    // ==================== 修改部分结束 ====================
}

// ==================== 修改部分开始 ====================
void TheButton::onJumpToClicked() {
    emit jumpTo(info);
}
// ==================== 修改部分结束 ====================
