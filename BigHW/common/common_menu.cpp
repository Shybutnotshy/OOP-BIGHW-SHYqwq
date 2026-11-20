/* BigHW */

/* ----------------------------------------------------------------------------------

     本文件功能：
	1、放各项目共用的菜单显示函数及单字符选择函数
	2、汉诺塔（数字选项，区分大小写无意义）与彩球游戏（字母选项，区分大小写）共用本文件的函数
	3、菜单项文本与合法字符集均由调用者传入，本文件不含任何具体项目的菜单内容

     本文件要求：
	1、不允许定义全局变量（const及#define不在限制范围内）
	2、不直接输出任何项目的菜单文字，保证可被后续各次大作业复用
	3、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

#include <iostream>
#include <cstring>
#include <conio.h>
using namespace std;

#include "../include/common_menu.h"

/***************************************************************************
  函数名称：Menu_toupper
  功    能：将小写字母转换成大写字母，非小写字母的字符原样返回
  输入参数：const char ch：待转换的字符
  返 回 值：转换后的字符
  说    明：菜单选项不区分大小写时、以及各项目规范化用户输入时都调用此函数
***************************************************************************/
char Menu_toupper(const char ch)
{
	if ('a' <= ch && ch <= 'z') {
		return (char)(ch - 'a' + 'A');
	}
	return ch;
}

/***************************************************************************
  函数名称：Menu_show
  功    能：显示一个菜单块：首尾各一条分隔线，中间逐行显示各菜单项，最后显示提示串
  输入参数：const char* const* item_list：菜单项文本数组
		   const int item_count        ：菜单项个数
		   const char* prompt          ：末尾的提示串，传 NULL 则不显示
  返 回 值：无
  说    明：分隔线的长度取最长菜单项的字节数（中文为双字节，一个汉字正好占两列），
		   与终端缺省字体下的实际显示宽度一致，菜单内容变化时无需手工调整
***************************************************************************/
void Menu_show(const char* const* item_list, const int item_count, const char* prompt)
{
	int max_len = 0;
	int i;

	/* 统计最长菜单项的长度，作为分隔线的长度 */
	for (i = 0; i < item_count; i++) {
		if (!item_list[i]) {
			continue;
		}
		int len = (int)strlen(item_list[i]);
		if (len > max_len) {
			max_len = len;
		}
	}
	if (max_len < 1) {
		max_len = 1;
	}

	for (i = 0; i < max_len; i++) {
		cout << '-';
	}
	cout << endl;

	for (i = 0; i < item_count; i++) {
		cout << item_list[i] << endl;
	}

	for (i = 0; i < max_len; i++) {
		cout << '-';
	}
	cout << endl;

	if (prompt) {
		cout << prompt;
	}

	return;
}

/***************************************************************************
  函数名称：Menu_getch
  功    能：等待用户按下一个属于 accept 的可见字符，按到非法键则继续等待
  输入参数：const char* accept ：允许的字符集合，例如 "0123456789"、"ABCDEFGQ"
		   const int ignore_case：非 0 表示不区分大小写（accept 中只需写大写字母）
  返 回 值：用户按下的字符（不区分大小写时已转换成大写）
  说    明：1、直接读键盘，不需要按回车，所以一次按键即可完成选择
		   2、功能键（方向键等）的第一个字节为 '\0'，直接忽略，不会误判为选项
		   3、accept 中除字母、数字外还可以放其它字符，只要有需要就写进去即可
***************************************************************************/
char Menu_getch(const char* accept, const int ignore_case)
{
	char ch;

	while (true) {
		ch = (char)_getch();
		if (ch == '\0') {
			continue;
		}
		if (ignore_case) {
			ch = Menu_toupper(ch);
		}
		if (strchr(accept, ch) != NULL) {
			return ch;
		}
	}
}

/***************************************************************************
  函数名称：Menu_select
  功    能：显示菜单并返回用户选中的字符
  输入参数：参见 Menu_show 与 Menu_getch
  返 回 值：用户选中的字符
  说    明：把"显示"与"选择"两步合成一次调用，各项目只需准备自己的菜单项文本
***************************************************************************/
char Menu_select(const char* const* item_list, const int item_count, const char* prompt,
                 const char* accept, const int ignore_case)
{
	Menu_show(item_list, item_count, prompt);
	return Menu_getch(accept, ignore_case);
}
