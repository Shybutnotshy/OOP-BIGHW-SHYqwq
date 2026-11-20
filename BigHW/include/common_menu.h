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

/* 将小写字母转换成大写字母，其它字符原样返回（各项目的输入规范化也用它） */
char Menu_toupper(const char ch);

/* 显示菜单：首尾各一条分隔线，中间逐行显示菜单项，最后显示提示串，不等待输入 */
void Menu_show(const char* const* item_list, const int item_count, const char* prompt);

/* 等待用户按下一个属于 accept 的字符（不需要按回车），ignore_case 非 0 时不区分大小写 */
char Menu_getch(const char* accept, const int ignore_case);

/* 显示菜单并返回用户选中的字符（= Menu_show + Menu_getch） */
char Menu_select(const char* const* item_list, const int item_count, const char* prompt,
                 const char* accept, const int ignore_case);
