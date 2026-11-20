/* BigHW */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <tchar.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "../include/cmd_console_tools.h"
#include "../include/common_menu.h"
#include "90-01-b2-color_linez.h"

/* ----------------------------------------------------------------------------------

     本文件功能：
	1、放用内部数组方式实现的各函数：初始化数组、显示数组、读入用户坐标、单步移动
	2、本文件只处理数组及字符界面，不涉及 cmd 伪图形界面（见 90-01-b2-color_linez_graph.cpp）

     本文件要求：
	1、不允许定义全局变量（const及#define不在限制范围内）
	2、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

// 初始化数组

/***************************************************************************
  函数名称：Base_init_grid
  功    能：把 line*column 的数组全部清零，再随机放入 init_balls 个彩球
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组，0 表示空，1-MAX_COLOR 表示球的颜色
		   const int line  ：行数
		   const int column：列数
		   const int init_balls：初始放入的球数
  返 回 值：无
  说    明：先清空数组，避免上一次残留的数据影响本次
***************************************************************************/
void Base_init_grid(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int init_balls)
{
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < column; j++) {
			grid[i][j] = 0;
		}
	}
	int buf[50];
	Grid_generate_balls(buf, init_balls);
	Grid_add_balls(grid, line, column, buf, init_balls);
}

// 打印内部数组

/***************************************************************************
  函数名称：Base_show_grid
  功    能：打印内部数组：先打印行号列号，再按行列打印各格的内容
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
		   const bool show_background：非0 时给有球的格子加上对应颜色，便于观察
		   const char* str ：数组的标题，如"初始数组"
  返 回 值：无
  说    明：有球的格子用 cct_showch 按颜色显示，显示完再把颜色恢复成缺省值
***************************************************************************/
void Base_show_grid(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const bool show_background, const char* str)
{
	int x, y;
	cct_getxy(x, y);
	x += 5, y += 3;
	cout << str << "：" << endl;
	cout << "  |";
	for (int i = 1; i <= column; i++) {
		cout << setw(3) << i;
	}
	cout << endl << "--+";
	for (int i = 0; i < column; i++) {
		cout << "---";
	}
	cout << "-" << endl;
	for (int i = 0; i < line; i++) {
		cout << (char)('A' + i) << " |";
		for (int j = 0; j < column; j++) {
			cout << "  ";
			if (show_background) {
				cct_showch(x + j * 3, y + i, (char)('0' + grid[i][j]), (grid[i][j] ? COLOR_HYELLOW : COLOR_BLACK), COLOR_MAP[grid[i][j]]);
				cct_setcolor(COLOR_BLACK, COLOR_WHITE);
			}
			else {
				cout << grid[i][j];
			}
		}
		cout << endl;
	}
}

/***************************************************************************
  函数名称：Base_show_buf
  功    能：打印"下3个彩球的颜色分别是"这一行，告诉用户接下来会出现的球
  输入参数：int* buf：长度为3的彩球颜色缓冲区
  返 回 值：无
  说    明：只显示前3个，与菜单中"下3个彩球"的说法一致
***************************************************************************/
void Base_show_buf(int *buf)
{
	cout << endl;
	cout << "下3个彩球的颜色分别是：";
	for (int i = 0; i < 3; i++) {
		cout << buf[i] << " ";
	}
	cout << endl;
}

// 菜单BC的步骤输入

/***************************************************************************
  函数名称：Base_get_query
  功    能：读入用户输入的"字母+数字"形式的坐标，并检查是否合法
  输入参数：bool flag：0 表示要输入的是要移动的球（矩阵），1 表示要输入的是目的位置
		   const int grid[MAX_SIZE][MAX_SIZE]：内部数组，用于判断该位置是否为空
		   const int line  ：行数
		   const int column：列数
		   int& x、int& y ：带回读入的坐标（行、列，均从0开始）
  返 回 值：无
  说    明：输入不合法、位置为空或位置非空时都提示并重新输入，直到合法为止
***************************************************************************/
void Base_get_query(bool flag, const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int &x, int &y) 
{
	int X, Y;
	cct_getxy(X, Y);
	while (true) {
		cct_gotoxy(X, Y);
		cout << setw(100) << ' ' << '\r';
		cout << "请以字母+数字形式[例：c2]输入要移动球的" << (flag ? "目的" : "矩阵") << "坐标：";
		
		char str[10], * p = str - 1;
		while ((*(++p) = (char)getchar()) != '\n') {
			*p = Menu_toupper(*p);
		}
		*p = '\0';

		if ((p - str) != 2 || str[0] < 'A' || str[0] >= 'A' + line || str[1] < '1' || str[1] > '0' + column) {
			cout << setw(100) << ' ' << '\r';
			cout << "输入非法，请重新输入";
			continue;
		}
		int posx = str[0] - 'A', posy = str[1] - '1';
		if (!flag && !grid[posx][posy]) {
			cout << setw(100) << ' ' << '\r';
			cout << "输入位置为空，请重新输入";
			continue;
		}
		if (flag && grid[posx][posy]) {
			cout << setw(100) << ' ' << '\r';
			cout << "输入位置非空，请重新输入";
			continue;
		}
		cout << setw(100) << ' ' << '\r';
		cout << "输入为" << str[0] << "行" << str[1] << "列" << endl;
		x = posx, y = posy;
		return;
	}
}

// 文字形式的单步演示

/***************************************************************************
  函数名称：Base_single_step
  功    能：完成一次移动：读入起点与终点，若能连通则把球搬过去
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
  返 回 值：true 表示移动成功，false 表示无法找到移动路径
  说    明：用 Grid_find_path 判断两点之间是否有通路，有则改数组内容
***************************************************************************/
bool Base_single_step(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column) 
{
	int fromx, fromy, tox, toy;

	Base_get_query(0, grid, line, column, fromx, fromy);
	Base_get_query(1, grid, line, column, tox, toy);

	int step_mem[100], * p = step_mem;
	if (Grid_find_path(grid, line, column, fromx, fromy, tox, toy, step_mem)) {
		grid[tox][toy] = grid[fromx][fromy];
		grid[fromx][fromy] = 0;
		return 1;
	}
	else {
		cout << "无法找到移动路径" << endl << endl;
		Base_show_grid(grid, line, column);
		return 0;
	}

}
