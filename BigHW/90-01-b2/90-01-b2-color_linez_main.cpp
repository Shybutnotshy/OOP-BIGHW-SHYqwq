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
	1、放main函数，按菜单选项调用 90-01-b2-color_linez_base.cpp / _graph.cpp / _tools.cpp 中的函数
	2、准备本项目的菜单项文本，调用公共菜单函数 common_menu.cpp 中的 Menu_show / Menu_getch
	   显示菜单并返回选项（菜单的显示与选择机制已提炼为公共函数，本文件只放菜单内容）

     本文件要求：
	1、不允许定义全局变量（含外部全局和静态全局，const及#define不在限制范围内）
	2、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

/* 彩球的菜单项 */
static const char* const COLOR_MENU_ITEM[] = {
	"A.内部数组，随机生成初始5个球",
	"B.内部数组，随机生成60% 的球，寻找移动路径",
	"C.内部数组，完整版",
	"D.画出n* n的框架（无分隔线），随机显示5个球",
	"E.画出n* n的框架（有分隔线），随机显示5个球",
	"F.n* n的框架，60% 的球，支持鼠标，完成一次移动",
	"G.cmd图形界面完整版",
	"Q.退出"
};

static const int COLOR_MENU_ITEM_COUNT =
	(int)(sizeof(COLOR_MENU_ITEM) / sizeof(COLOR_MENU_ITEM[0]));

static const char COLOR_MENU_ACCEPT[] = "ABCDEFGQ";

/***************************************************************************
  函数名称：Menu_get_mod
  功    能：清屏、显示彩球菜单，并等待用户按下字母选项后返回
  输入参数：无
  返 回 值：用户选中的选项（0-6 对应 A-G），-1 表示选择了 Q（退出）
  说    明：菜单显示与单字符选择都调用公共函数，本函数只提供菜单内容；
		   不区分大小写，因此用户输入小写字母也能正确选择
***************************************************************************/
static int Menu_get_mod(void)
{
	cct_cls();
	char ch = Menu_select(COLOR_MENU_ITEM, COLOR_MENU_ITEM_COUNT, "[请选择:] ", COLOR_MENU_ACCEPT, 1);

	if (ch == 'Q') {
		return -1;
	}
	return ch - 'A';
}

/***************************************************************************
  函数名称：main
  功    能：程序入口：设置窗口与字体后，循环"显示菜单 → 执行选中项"，直到用户选择退出
  输入参数：无
  返 回 值：0
  说    明：各选项的具体实现分散在 base / graph / tools 三个私有源文件中
***************************************************************************/
int main()
{

	/* 这是一段用于验证cmd_console_tools.cpp版本是否正确的代码，判断正确的具体标准在作业文档中 */

	/*
	cct_showstr(10, 2, "请输出雪花的大小", COLOR_BLACK, COLOR_WHITE, 2);
	cout << endl << endl;
	cct_showstr(10, 4, "╔═══汉诺塔═══╗", COLOR_BLACK, COLOR_WHITE, 1);
	cct_showstr(10, 5, "╔═══汉诺塔 ═══╗", COLOR_WHITE, COLOR_BLACK, 1);
	cct_showstr(10, 6, "╔═══H汉诺塔 ═══╗", COLOR_RED, COLOR_HYELLOW, 1);

	cct_showstr(10, 8,  "**╔═╦═╗中", COLOR_CYAN, COLOR_YELLOW, 1);
	cct_showstr(10, 9,  "中║测║试║**", COLOR_HBLUE, COLOR_HPINK, 1);
	cct_showstr(10, 10, "**╠═╬═╣**", COLOR_YELLOW, COLOR_HGREEN, 1);
	cct_showstr(10, 11, "中║ab║12║中", COLOR_HPINK, COLOR_HCYAN, 1);
	cct_showstr(10, 12, "**╚═╩═╝中", COLOR_GREEN, COLOR_HBLUE, 1);

	to_be_continued("请确认上面的输出没有乱码/字符相互重叠现象，如果有，说明cmd_console_tools.cpp不对");
	*/

    //给出后续的正常内容

	int line, column;
	int grid[MAX_SIZE][MAX_SIZE];

	Random_init();

	cct_setconsoletitle("Color_linez");
	cct_setconsoleborder(80, 28, 80, 200);
	cct_setfontsize("新宋体", 28);

	while (true) {
		int mod = Menu_get_mod();

		if (mod == -1) { // 输入Q退出
			cct_cls();
			End_of_case();
			to_be_continued("程序结束");
			return 0;
		}

		cct_cls();
		while (true) {
			cout << "请输入行数(7-9)：" << endl;
			cin >> line;
			if (Input_check_int(line, 7, 9)) {
				cin.ignore(10000, '\n');
				break;
			}
			cin.ignore(10000, '\n');
		}

		while (true) {
			cout << "请输入列数(7-9)：" << endl;
			cin >> column;
			if (Input_check_int(column, 7, 9)) {
				cin.ignore(10000, '\n');
				break;
			}
			cin.ignore(10000, '\n');
		}

		#ifdef ADMIN

		

		#endif

		if (mod == 0) {
			Base_init_grid(grid, line, column);
			Base_show_grid(grid, line, column, 1, "初始数组");
			End_of_case();
		}
		else if (mod == 1) {

			Base_init_grid(grid, line, column, (int)(line * column * 0.6));
			Base_show_grid(grid, line, column, 1, "初始数组");

			int buf[5];
			Grid_generate_balls(buf);
			Base_show_buf(buf);

			int fromx, fromy, tox, toy;

			Base_get_query(0, grid, line, column, fromx, fromy);
			Base_get_query(1, grid, line, column, tox, toy);

			int step_mem[100], *p = step_mem;
			if (Grid_find_path(grid, line, column, fromx, fromy, tox, toy, step_mem)) {
				int X, Y;
				
				int empty_grid[MAX_SIZE][MAX_SIZE] = { 0 };

				cout << endl;

				cct_getxy(X, Y);
				Base_show_grid(empty_grid, line, column, 0, "查找结果");

				for (; *p != -1; p++) {
					int i, j;
					Calc_xy(*p, i, j);
					int nx = X + j * 3 + 5, ny = Y + i + 3;
					cct_gotoxy(nx, ny);
					cout << "*";
				}
				cct_gotoxy(X, Y + line + 3);

				cout << endl;
				cct_getxy(X, Y);

				Base_show_grid(grid, line, column, 0, "路径演示");
				p = step_mem;
				int ncolor = COLOR_MAP[grid[fromx][fromy]];
				for (; *p != -1; p++) {
					int i, j;
					Calc_xy(*p, i, j);
					int nx = X + j * 3 + 5, ny = Y + i + 3;
					cct_showch(nx, ny, (char)('0' + grid[i][j]), COLOR_HYELLOW, ncolor);
					cct_setcolor(COLOR_BLACK, COLOR_WHITE);
				}
				cct_gotoxy(X, Y + line + 3);

				cout << endl;
			}
			else {
				cout << "无法找到移动路径" << endl;
			}

			End_of_case();
		}
		else if (mod == 2) {
			Base_init_grid(grid, line, column);
			Base_show_grid(grid, line, column, 1, "初始数组");
			int buf[5], score = 0;

			Grid_generate_balls(buf);
			Base_show_buf(buf);

			while (true) {
				while (!Base_single_step(grid, line, column))
					;
				
				int nscore = Grid_calc_score(grid, line, column);
				score += nscore;

				if (!nscore) {
					if (!Grid_add_balls(grid, line, column, buf)) {
						cout << endl << "游戏结束！你的总得分为 " << score << endl;
						break;
					}
					nscore = Grid_calc_score(grid, line, column);
					score += nscore;
					Grid_generate_balls(buf);
					if (Grid_filled(grid, line, column)) {
						cout << endl << "游戏结束！你的总得分为 " << score << endl;
						break;
					}
				}

				cout << endl << "本次得分: " << nscore << ", 总得分: " << score << endl << endl;
				Base_show_grid(grid, line, column);
				Base_show_buf(buf);
			}

			End_of_case();
		}
		else if (mod == 3) {
			Base_init_grid(grid, line, column);
			Base_show_grid(grid, line, column, 1, "初始数组");
			int x, y;
			cct_getxy(x, y);
			to_be_continued("初始数组显示完成", x, y);
			Graph_show_border(line, column, 0);
			Graph_show_grid(grid, line, column, 0);
			End_of_case();
		}
		else if (mod == 4) {
			Base_init_grid(grid, line, column);
			Base_show_grid(grid, line, column, 1, "初始数组");
			int x, y;
			cct_getxy(x, y);
			to_be_continued("初始数组显示完成", x, y);
			Graph_show_border(line, column);
			Graph_show_grid(grid, line, column);
			End_of_case();
		}
		else if (mod == 5) {
			Base_init_grid(grid, line, column, (int)(line* column * 0.6));
			Grid_calc_score(grid, line, column);
			Graph_show_border(line, column);
			Graph_show_grid(grid, line, column);
			cct_gotoxy(0, 0);
			cout << "鼠标移动，左键选择，右键取消";
			Graph_single_step(grid, line, column);
			Grid_calc_score(grid, line, column);
			Graph_show_grid(grid, line, column);
			End_of_case();
		}
		else if (mod == 6) {
			Base_init_grid(grid, line, column);
			Graph_show_border(line, column);
			Graph_show_grid(grid, line, column);
			cct_gotoxy(0, 0);
			cout << "鼠标移动，左键选择，右键取消";

			int buf[5], score = 0;
			int color_cnt[MAX_COLOR + 1] = { 0 };

			Grid_generate_balls(buf);
			Graph_show_state(grid, line, column, buf, color_cnt, score);
			
			while (true) {
				while (!Graph_single_step(grid, line, column))
					;

				int nscore = Grid_calc_score(grid, line, column, color_cnt);
				score += nscore;

				if (!nscore) {
					if (!Grid_add_balls(grid, line, column, buf)) {
						cct_gotoxy(0, column * 2 + 5);
						cout << "游戏结束！" << endl;
						break;
					}
					nscore = Grid_calc_score(grid, line, column, color_cnt);
					score += nscore;
					Grid_generate_balls(buf);
					if (Grid_filled(grid, line, column)) {
						cct_gotoxy(0, column * 2 + 5);
						cout << "游戏结束！" << endl;
						break;
					}
				}


				Graph_show_grid(grid, line, column);
				Graph_show_state(grid, line, column, buf, color_cnt, score);
			}


			End_of_case();
		}
	}

    return 0;
}
/*

A.内部数组，随机生成初始5个球
B.内部数组，随机生成60%的球，寻找移动路径
C.内部数组，完整版
D.画出n*n的框架（无分隔线），随机显示5个球
E.画出n*n的框架（有分隔线），随机显示5个球
F.n*n的框架，60%的球，支持鼠标，完成一次移动
G.cmd图形界面完整版

*/
