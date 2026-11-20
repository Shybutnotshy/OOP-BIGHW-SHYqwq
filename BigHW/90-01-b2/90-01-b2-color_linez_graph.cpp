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
#include "90-01-b2-color_linez.h"

/* ----------------------------------------------------------------------------------

     本文件功能：
	1、放用 cmd 伪图形界面方式实现的各函数：画框架、画球、鼠标选球、移动动画、状态栏
	2、本文件只负责显示与交互，数组的逻辑判断都调用 _base.cpp / _tools.cpp 中的函数

     本文件要求：
	1、不允许定义全局变量（const及#define不在限制范围内）
	2、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

/* 制表符字符串表：定义为 static const，避免成为可修改的全局变量 */
static const char* const BORDER[] = { "╔", "╚", "╗", "╝", "═", "║", "╦", "╩", "╠", "╣", "╬" };

#define NW_CORNER   BORDER[0]
#define SW_CORNER   BORDER[1]
#define NE_CORNER   BORDER[2]
#define SE_CORNER   BORDER[3]
#define PAR_LINE    BORDER[4]
#define VER_LINE    BORDER[5]
#define DOWN_T      BORDER[6]
#define UP_T        BORDER[7]
#define RIGHT_T     BORDER[8]
#define LEFT_T      BORDER[9]
#define CROSS       BORDER[10]
#define BALL        "〇"
#define CHOSEN_BALL "◎"

// 打印边框 （是否含内框）

/***************************************************************************
  函数名称：Graph_show_border
  功    能：用中文制表符画出 line*column 的框架，可选择是否画分隔线
  输入参数：const int line  ：行数
		   const int column：列数
		   bool show_inner_border：非0 表示每格之间画分隔线
  返 回 值：无
  说    明：1、先清屏再画，保证框架位置固定
		   2、每格占 2 列，与后面画球、鼠标定位的换算保持一致
***************************************************************************/
void Graph_show_border(const int line, const int column, bool show_inner_border)
{
	cct_cls();
	cout << endl;
	cct_setcolor(COLOR_HWHITE, COLOR_BLACK);

	cout << NW_CORNER << " ";
	for (int i = 0; i < column; i++) {
		cout << PAR_LINE << " ";
		if (show_inner_border && i < column - 1) {
			cout << DOWN_T << " ";
		}
	}
	cout << NE_CORNER << " " << endl;

	for (int i = 0; i < line; i++) {
		cout << VER_LINE << " ";
		for (int j = 0; j < column; j++) {
			cout << "  ";
			if (show_inner_border && j < column - 1) {
				cout << VER_LINE << " ";
			}
		}
		cout << VER_LINE << " " << endl;

		if (show_inner_border && i < line - 1) {
			cout << RIGHT_T << " ";
			for (int j = 0; j < column; j++) {
				cout << PAR_LINE << " ";
				if (j < column - 1) {
					cout << CROSS << " ";
				}
			}
			cout << LEFT_T << " " << endl;
		}
	}

	cout << SW_CORNER << " ";
	for (int i = 0; i < column; i++) {
		cout << PAR_LINE << " ";
		if (show_inner_border && i < column - 1) {
			cout << UP_T << " ";
		}
	}
	cout << SE_CORNER << " " << endl;
	cct_setcolor();
}

// 打印小球（是否含内框）

/***************************************************************************
  函数名称：Graph_show_grid
  功    能：把内部数组中的彩球按颜色显示到框架的对应格子里
  输入参数：const int grid[MAX_SIZE][MAX_SIZE]：内部数组，0 表示空格
		   const int line  ：行数
		   const int column：列数
		   bool show_inner_border：非0 表示框架有分隔线，格子间距要相应变化
  返 回 值：无
  说    明：有分隔线时每格横向占 4 列、纵向占 2 行，无分隔线时横向占 2 列、纵向占 1 行
***************************************************************************/
void Graph_show_grid(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, bool show_inner_border)
{
	const int X = 2, Y = 2;
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < column; j++) {
			int nx = X + j * 2 * (1 + show_inner_border), ny = Y + i * (1 + show_inner_border);
			cct_showstr(nx, ny, BALL, COLOR_MAP[grid[i][j]], COLOR_HWHITE);
		}
	}
	cct_setcolor();
	cct_gotoxy(0, Y + column * (1 + show_inner_border) + 1);
}

// 将鼠标位置映射为图中坐标

/***************************************************************************
  函数名称：Check_position
  功    能：把鼠标的字符坐标换算成图中的行列号，并判断该位置是否正好落在某一格内
  输入参数：const int line  ：行数
		   const int column：列数
		   const int mouse_x、mouse_y：鼠标的字符坐标
		   int &x、int &y：带回图中的行号与列号
  返 回 值：true 表示鼠标在某一格的范围内，false 表示在框架外或落在分隔线上
  说    明：有分隔线时每格横向占 4 列、纵向占 2 行，取余即可判断是否压在分隔线上
***************************************************************************/
bool Check_position(const int line, const int column, const int mouse_x, const int mouse_y, int &x, int &y)
{
	const int X = 2, Y = 2;

	y = (mouse_x - X) / 4, x = (mouse_y - Y) / 2;

	if (mouse_x < X || mouse_y < Y || !In_grid(x, y, line, column)) {
		return 0;
	}
	if ((mouse_x - X) - 4 * y > 1 || (mouse_y - Y) - 2 * x) {
		return 0;
	}
	return 1;
}

/***************************************************************************
  函数名称：Graph_move
  功    能：把球从一格移动到相邻一格的动画：先擦起点、再在中间点显示、最后落到终点
  输入参数：const int fromx、fromy：起点行列号
		   const int tox、toy    ：终点行列号
		   const int color        ：球的颜色
  返 回 值：无
  说    明：共分两帧（中间点、终点），每帧之间延时 100ms，看起来像球在滚动
***************************************************************************/
void Graph_move(const int fromx, const int fromy, const int tox, const int toy, const int color)
{
	const int X = 2, Y = 2;
	int stepy = (tox - fromx) * 2, stepx = (toy - fromy) * 4;
	int nowx = X + fromy * 4, nowy = Y + fromx * 2;
	
	int newx = nowx + stepx, newy = nowy + stepy;
	cct_delay(100);
	cct_showstr((newx + nowx) / 2, (newy + nowy) / 2, BALL, color, COLOR_HWHITE);
	cct_showstr(nowx, nowy, BALL, COLOR_HWHITE, COLOR_HWHITE);

	cct_delay(100);
	cct_showstr(newx, newy, BALL, color, COLOR_HWHITE);
	cct_showstr((newx + nowx) / 2, (newy + nowy) / 2, (stepx ? VER_LINE : PAR_LINE), COLOR_HWHITE, COLOR_BLACK);

	cct_setcolor();
}

/***************************************************************************
  函数名称：Graph_single_step
  功    能：用鼠标完成一次移动：左键选起点、再左键选终点，右键取消选择
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组（移动成功后内容被更新）
		   const int line  ：行数
		   const int column：列数
  返 回 值：true 表示移动成功，false 表示两点之间没有通路
  说    明：1、选中起点后球变成 CHOSEN_BALL，取消或移动后恢复成 BALL
		   2、找到通路时按路径逐格播放移动动画
***************************************************************************/
bool Graph_single_step(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column)
{
	const int X = 2, Y = 2;
	cct_enable_mouse();
	cct_setcursor(CCT_CURSOR_INVISIBLE);

	int fromx, fromy, tox, toy;
	int nowx, nowy;
	int step_mem[100], *p;

	bool start_chosen = 0;

	while (true) {
		int mouse_x = 0, mouse_y = 0, mouse_event, key1, key2;
		cct_read_keyboard_and_mouse(mouse_x, mouse_y, mouse_event, key1, key2);
		int x, y;

		if (mouse_event == MOUSE_RIGHT_BUTTON_CLICK) {
			if (start_chosen) {
				start_chosen = 0;
				int nx = X + fromy * 4, ny = Y + fromx * 2;
				cct_showstr(nx, ny, BALL, COLOR_MAP[grid[fromx][fromy]], COLOR_HWHITE);
				cct_setcolor();
			}
		}

		cct_gotoxy(0, Y + column * 2 + 1);
		cout << "当前鼠标位置： ";
		if (!Check_position(line, column, mouse_x, mouse_y, x, y)) {
			cout << "[位置非法]    ";
			continue;
		}

		cout << (char)('A' + x) << "行" << y + 1 << "列      ";

		if (mouse_event == MOUSE_LEFT_BUTTON_CLICK) {
			if (start_chosen) {
				if (!grid[x][y]) {
					tox = x, toy = y;
					break;
				}
			}
			else {
				if (grid[x][y]) {
					fromx = x, fromy = y;
					start_chosen = 1;
					int nx = X + y * 4, ny = Y + x * 2;
					cct_showstr(nx, ny, CHOSEN_BALL, COLOR_MAP[grid[x][y]], COLOR_HWHITE);
					cct_setcolor();
				}
			}
		}
	}

	int nx = X + fromy * 4, ny = Y + fromx * 2;
	cct_showstr(nx, ny, CHOSEN_BALL, COLOR_MAP[grid[fromx][fromy]], COLOR_HWHITE);
	cct_setcolor();

	nowx = fromx, nowy = fromy;
	bool flag = Grid_find_path(grid, line, column, fromx, fromy, tox, toy, step_mem);

	if (flag) {
		cct_gotoxy(0, Y + column * 2 + 2);
		cout << setw(20) << ' ' << '\r';
		cout << "可以从[" << (char)('A' + fromx) << (char)('1' + fromy) << "]移动到[" << (char)('A' + tox) << (char)('1' + toy) << "]";

		for (p = step_mem + 1; *p != -1; p++) {
			int newx, newy;
			Calc_xy(*p, newx, newy);
			Graph_move(nowx, nowy, newx, newy, COLOR_MAP[grid[fromx][fromy]]);
			nowx = newx, nowy = newy;
		}
		grid[tox][toy] = grid[fromx][fromy];
		grid[fromx][fromy] = 0;
		
		cct_delay(500);
	}
	else {
		cct_gotoxy(0, Y + column * 2 + 2);
		cout << setw(20) << ' ' << '\r';
		cout << "无法移动，请重新选择";
		cct_delay(500);
	}

	cct_setcursor(CCT_CURSOR_VISIBLE_NORMAL);
	cct_disable_mouse();
	return flag;
}

// 显示右侧状态栏

/***************************************************************************
  函数名称：Graph_show_state
  功    能：显示右侧的状态：当前得分、下3个彩球的颜色、各种颜色剩余的个数与占比
  输入参数：const int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
		   const int* buf  ：下3个彩球的颜色
		   const int* color_cnt：各颜色累计被消去的个数
		   const int score ：当前总分
  返 回 值：无
  说    明：状态栏固定在屏幕右侧，每次刷新时重画，不会与左侧的框架重叠
***************************************************************************/
void Graph_show_state(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int* buf, const int* color_cnt, const int score)
{
	const int SCORE_X = 40, SCORE_Y = 1;
	const int BUF_X = 40, BUF_Y = 5;
	const int SHEET_X = 40, SHEET_Y = 9;

	if (1) {
		cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
		cct_gotoxy(SCORE_X, SCORE_Y);
		cout << "╔ ═ ═ ═ ═ ═ ╗ ";
		cct_gotoxy(SCORE_X, SCORE_Y + 1);
		cout << "║ 得分：" << setw(4) << score << "║ ";
		cct_gotoxy(SCORE_X, SCORE_Y + 2);
		cout << "╚ ═ ═ ═ ═ ═ ╝ ";
		cct_setcolor();
	}

	if (1) {
		cct_showstr(BUF_X, BUF_Y + 0, "╔═╦═╦═╗", COLOR_HWHITE, COLOR_BLACK);
		cct_showstr(BUF_X, BUF_Y + 1, "║  ║  ║  ║", COLOR_HWHITE, COLOR_BLACK);
		cct_showstr(BUF_X, BUF_Y + 2, "╚═╩═╩═╝", COLOR_HWHITE, COLOR_BLACK);
		for (int i = 0; i < 3; i++) {
			cct_showstr(BUF_X + 2 + i * 4, BUF_Y + 1, BALL, COLOR_MAP[buf[i]], COLOR_HWHITE);
		}
		cct_setcolor();
	}
	
	if (1) {
		int cnt[MAX_COLOR + 1] = { 0 };

		for (int i = 0; i < line; i++) {
			for (int j = 0; j < column; j++) {
				cnt[grid[i][j]]++;
			}
		}

		cct_showstr(SHEET_X, SHEET_Y, "╔════════════╗", COLOR_HWHITE, COLOR_BLACK);
		for (int i = 0; i <= MAX_COLOR; i++) {
			cct_showstr(SHEET_X, SHEET_Y + i + 1, "║", COLOR_HWHITE, COLOR_BLACK);
			cct_showstr(SHEET_X + 2, SHEET_Y + i + 1, BALL, COLOR_MAP[i], COLOR_HWHITE);
			cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
			double percent = 1.0 * cnt[i] / (line * column) * 100;
			cout << ":" << setfill('0') << setw(2) << cnt[i] << "/(";
			cout << setfill(' ') << setw(5) << fixed << setprecision(2) << percent;
			cout << "%) 消除-" << setw(3) << color_cnt[i];
			cout << " ║ ";
		}
		cct_showstr(SHEET_X, SHEET_Y + MAX_COLOR + 2, "╚════════════╝", COLOR_HWHITE, COLOR_BLACK);
		cct_setcolor();
	}
}
