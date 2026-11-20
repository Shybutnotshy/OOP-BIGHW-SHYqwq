/* BigHW */

#include <conio.h>   //本源程序允许使用，因为要_getch()
#include <iostream>
#include <Windows.h>
using namespace std;

//其余需要的头文件，按需加入，不要违规

#include "../include/cmd_console_tools.h"
#include "../include/common_menu.h"
#include "90-01-b1-hanoi.h"

/* ----------------------------------------------------------------------------------

     本文件功能：
	1、放main函数
	2、初始化屏幕
	3、准备本项目的菜单项文本，调用公共菜单函数 common_menu.cpp 中的 Menu_show / Menu_getch
	   显示菜单并返回选项（菜单的显示与选择机制已提炼为公共函数，本文件只放菜单内容）
	4、根据选项调用菜单各项对应的执行函数（90-01-b1-hanoi_multiple_solutions.cpp 中）

     本文件要求：
	1、不允许定义全局变量（含外部全局和静态全局，const及#define不在限制范围内）
	2、静态局部变量的数量不限制，但使用准则也是：少用、慎用、能不用尽量不用
	3、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

/* 汉诺塔的菜单项 */
static const char* const HANOI_MENU_ITEM[] = {
	"1.基本解",
	"2.基本解(步数记录)",
	"3.内部数组显示(横向)",
	"4.内部数组显示(纵向+横向)",
	"5.图形解-预备-画三个圆柱",
	"6.图形解-预备-在起始柱上画n个盘子",
	"7.图形解-预备-第一次移动",
	"8.图形解-自动移动版本",
	"9.图形解-游戏版",
	"0.退出"
};

static const int HANOI_MENU_ITEM_COUNT =
	(int)(sizeof(HANOI_MENU_ITEM) / sizeof(HANOI_MENU_ITEM[0]));

static const char HANOI_MENU_ACCEPT[] = "0123456789";

/***************************************************************************
  函数名称：Menu_get_mod
  功    能：清屏、显示汉诺塔菜单，并等待用户按下数字选项后返回
  输入参数：无
  返 回 值：用户选中的选项（0-9，其中 0 表示退出）
  说    明：菜单显示与单字符选择都调用公共函数，本函数只提供菜单内容
***************************************************************************/
static int Menu_get_mod(void)
{
	cct_cls();
	Menu_show(HANOI_MENU_ITEM, HANOI_MENU_ITEM_COUNT, "[请选择:] ");

	/* 字符减去 '0' 即得到对应的数字选项 */
	return Menu_getch(HANOI_MENU_ACCEPT, 0) - '0';
}

/***************************************************************************
  函数名称：main
  功    能：程序入口：确认窗口设置后，循环"显示菜单 → 执行选中项"，直到用户选择退出
  输入参数：无
  返 回 值：0
  说    明：菜单的显示与选择由公共函数完成，本函数只负责转发选项
***************************************************************************/
int main()
{
	/* 将这段复制到main的最前面 */
	cout << "请确认当前cmd窗口的大小为40行*120列以上，字体为新宋体/16，按C继续，Q退出" << endl;
	while (1) {
		char ch = (char)_getch();
		if (ch == 'C' || ch == 'c')
			break;
		if (ch == 'Q' || ch == 'q')
			return 0;
	}

	/* 从这里继续你的程序 */

	while (true) {
		int mod = Menu_get_mod();

		int status = Run_hanoi(mod);

		if (status) {
			return 0;
		}
	}


	return 0;
}
