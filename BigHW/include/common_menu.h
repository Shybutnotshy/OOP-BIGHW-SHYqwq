/* BigHW */

#pragma once

/* ------------------------------------------------------------------------------------------------------

     本文件功能：
	1、声明 common\common_menu.cpp 中实现的公共菜单函数，供汉诺塔、彩球游戏等项目共用
	2、这些函数只解决"按单个字符选择某一项"这个共性，具体菜单内容是各项目的数据，由调用者传入
	3、因为菜单项文本及合法字符集都是形参，所以数字选项、字母选项、字母区分大小写、其它字符选项
	   都可以用同一套函数完成，不必每个项目各写一份菜单

     本文件要求：
	1、只放函数声明，不放函数定义，不放变量定义
	2、不允许把本文件拷贝到各项目目录中，各项目用相对路径包含公共头文件

   ------------------------------------------------------------------------------------------------------ */


char Menu_toupper(const char ch);
void Menu_show(const char* const* item_list, const int item_count, const char* prompt);
char Menu_getch(const char* accept, const int ignore_case);
char Menu_select(const char* const* item_list, const int item_count, const char* prompt,
                 const char* accept, const int ignore_case);
