/**
* @file PubDef.h
* @brief 基础公共宏
* @authors yandaoyang
* @date 2020/12/27
* @note 目前用于控制软或硬解码
*/

#pragma once

#define SAFE_DEL(ptr) if(ptr){delete ptr;ptr=nullptr;}


//#define HARDWARE_DECODE


/* 
* 目前Font Awesome字库桌面应用的图标对应的字符表可参考以下网站：
* http://www.fontawesome.com.cn/cheatsheet/
* 
* 目前还未实现（有想到的继续在这里补充）：
* 1、左、上、左上、左下、右上的拖拽改变窗口大小
* 2、暂停的时候seek，显示出seek后的目标画面，不改变暂停状态
* 3、视频停止、倍速、上一个、下一个、双击全屏/恢复、单击暂停/恢复
* 4、音量调整、截图拍照、设置界面、视频列表
* 5、标题栏标题、标题栏自动隐藏、启动动画、BaseWindow提供禁用拖拽的接口
* 6、init()应该为bool类型，且为CommonWindow的虚函数，由APP_MGR统一init
* 7、public的onBtnMaxClicked后期应改为req的请求信号，在MainPlayer的bindSlots中
* 8、最大化无操作后鼠标光标消失、鼠标光标在按钮上为手
* 9、avformat_open_input的interrupt
* 6、...
*/

