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
	1、放内部数组方式与 cmd 伪图形界面方式都要用到的公共函数（本项目的私有公共函数）
	2、随机数、坐标换算、路径搜索、得分计算、结束提示等都在这里
	3、判断字符大小写已提炼为大作业的公共函数 Menu_toupper（common_menu.cpp）

     本文件要求：
	1、不允许定义全局变量（const及#define不在限制范围内）
	2、按需加入系统头文件、自定义头文件、命名空间等

   ----------------------------------------------------------------------------------- */

/***************************************************************************
  函数名称：to_be_continued
  功    能：在指定位置显示提示串并等待用户按回车，用于各小题之间的停顿
  输入参数：const char* prompt：提示串，传 NULL 则只显示"按回车键继续..."
		   const int X、const int Y：提示串的显示位置
  返 回 值：无
  说    明：1、先把颜色恢复成缺省值，避免接着上一次的高亮颜色继续显示
		   2、只认回车键，其它按键都忽略
***************************************************************************/
void to_be_continued(const char* prompt, const int X, const int Y)
{
	cct_setcolor(); //恢复缺省颜色
	cct_gotoxy(X, Y);

	if (prompt)
		cout << prompt << "，按回车键继续...";
	else
		cout << "按回车键继续...";

	/* 忽略除回车键外的所有输入（注意：_getch的回车是\r，而getchar是\n）*/
	while (_getch() != '\r')
		;
	cout << endl;
	//cct_cls();

	return;
}

// 随机数生成

/***************************************************************************
  函数名称：Random_init
  功    能：用当前时间做随机数种子
  输入参数：无
  返 回 值：无
  说    明：只需在程序开始时调用一次，之后每次运行产生的球序都不同
***************************************************************************/
void Random_init()
{
	srand((unsigned int)(time(0)));
}

/***************************************************************************
  函数名称：Random_get
  功    能：取得一个 0 ~ lim-1 之间的随机整数
  输入参数：int lim：取值范围的上限（不含）
  返 回 值：0 ~ lim-1 之间的随机整数
  说    明：缺省上限为最大整数，调用时才取模，便于不同的调用者共用同一个函数
***************************************************************************/
int Random_get(int lim)
{
	return rand() % lim;
}

// 输入是否合法

/***************************************************************************
  函数名称：Input_check_int
  功    能：判断用 cin 读入的整数是否读成功、且落在 [l,r] 区间内
  输入参数：int x     ：刚刚读入的整数
		   const int l：区间下限
		   const int r：区间上限
  返 回 值：true 表示合法，false 表示非法
  说    明：读入失败时顺手清除 cin 的错误状态，让调用者可以重新读入
***************************************************************************/
bool Input_check_int(int x, const int l, const int r)
{
	if (cin.fail()) {
		cin.clear();
		return 0;
	}

	return (l <= x) && (x <= r);
}

// 状压坐标

/***************************************************************************
  函数名称：Calc_pos
  功    能：把二维坐标 (x,y) 压缩成一个一维整数，便于做数组下标或路径记录
  输入参数：const int x：行号
		   const int y：列号
  返 回 值：压缩后的位置值 x*MAX_SIZE+y
  说    明：与 Calc_xy 互为逆运算
***************************************************************************/
int Calc_pos(const int x, const int y)
{
	return x * MAX_SIZE + y;
}

/***************************************************************************
  函数名称：Calc_xy
  功    能：把 Calc_pos 压缩后的位置值还原成二维坐标
  输入参数：const int pos：压缩后的位置值
		   int& x、int& y：带回行号与列号
  返 回 值：无
  说    明：与 Calc_pos 互为逆运算
***************************************************************************/
void Calc_xy(const int pos, int& x, int& y)
{
	x = pos / MAX_SIZE, y = pos % MAX_SIZE;
	return;
}

// 生成n个小球和n个位置 / 放置小球 (缺省为3)

/***************************************************************************
  函数名称：Grid_generate_balls
  功    能：随机生成 n 个 1~MAX_COLOR 的彩球颜色，放入缓冲区
  输入参数：int* buf：存放颜色的缓冲区，长度不小于 n
		   const int n：生成的个数
  返 回 值：无
  说    明：颜色加 1 是为了避开 0（0 在数组中表示"空"）
***************************************************************************/
void Grid_generate_balls(int *buf, const int n)
{
	for (int i = 0; i < n; i++) {
		*(buf++) = Random_get(MAX_COLOR) + 1;
	}
}

/***************************************************************************
  函数名称：Grid_add_balls
  功    能：把缓冲区中的 n 个球随机放到数组的空格中
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
		   int* buf：存放待放置颜色的缓冲区
		   const int n：要放置的个数
  返 回 值：true 表示全部放置成功，false 表示空格不够
  说    明：先把所有空格的位置收集起来，再用随机下标挑空格，保证不会放到同一格
***************************************************************************/
bool Grid_add_balls(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int* buf, const int n)
{
	int empty[100] = { 0 }, empty_cnt = 0;
	bool visited[100] = { 0 };

	for (int i = 0; i < line; i++) {
		for (int j = 0; j < column; j++) {
			if (!grid[i][j]) {
				empty[empty_cnt++] = Calc_pos(i, j);
			}
		}
	}

	if (empty_cnt < n) {
		return 0;
	}

	int cnt = 0;
	while (cnt < n) {
		int pos = empty[Random_get(empty_cnt)];
		if (visited[pos]) {
			continue;
		}
		int x, y;
		Calc_xy(pos, x, y);
		grid[x][y] = *(buf++);
		visited[pos] = 1;
		cnt++;
	}
	return 1;
}

// 坐标是否在图内

/***************************************************************************
  函数名称：In_grid
  功    能：判断坐标 (X,Y) 是否落在 line*column 的图内
  输入参数：const int X、const int Y：待判断的坐标
		   const int line  ：行数
		   const int column：列数
  返 回 值：true 表示在图内，false 表示越界
  说    明：路径搜索时用来做边界检查
***************************************************************************/
bool In_grid(const int X, const int Y, const int line, const int column)
{
	return 0 <= X && X < line && 0 <= Y && Y < column;
}

// 图是否被填满

/***************************************************************************
  函数名称：Grid_filled
  功    能：判断 line*column 的数组是否已经全部被彩球填满
  输入参数：const int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
  返 回 值：true 表示已填满，false 表示还有空格
  说    明：填满且没有可消除的球时游戏结束
***************************************************************************/
bool Grid_filled(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column) 
{
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < column; j++) {
			if (!grid[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

// 路径搜索

/***************************************************************************
  函数名称：Grid_dfs
  功    能：深度优先搜索从 (x,y) 到 (tar_x,tar_y) 的一条通路，并记录路径
  输入参数：int x、int y：当前所在的位置
		   const int tar_x、const int tar_y：目标位置
		   const int grid[MAX_SIZE][MAX_SIZE]：内部数组，非0表示该格有球、不能通过
		   bool visited[MAX_SIZE][MAX_SIZE]：访问标记，避免走回头路
		   int line、int column：图的行数与列数
		   int* top：路径记录数组的当前写入位置，搜索成功时在末尾写入 -1
  返 回 值：true 表示找到通路，false 表示没有通路
  说    明：本函数是本文件的内部实现，不对外使用，故用 static 修饰
***************************************************************************/
static bool Grid_dfs(int x, int y, const int tar_x, const int tar_y, const int grid[MAX_SIZE][MAX_SIZE], bool visited[MAX_SIZE][MAX_SIZE], int line, int column, int *top) 
{
	const int DIRECTION[][2] = { {1, 0},{0, 1},{-1, 0},{0, -1} };
	*(top++) = Calc_pos(x, y);
	if (x == tar_x && y == tar_y) {
		*top = -1;
		return 1;
	}
	for (int i = 0; i < 4; i++) {
		int nx = x + DIRECTION[i][0], ny = y + DIRECTION[i][1];
		if (!In_grid(nx, ny, line, column) || grid[nx][ny] || visited[nx][ny]) {
			continue;
		}
		visited[nx][ny] = 1;
		if (Grid_dfs(nx, ny, tar_x, tar_y, grid, visited, line, column, top)) {
			return 1;
		}
	}
	return 0;
}

/***************************************************************************
  函数名称：Grid_find_path
  功    能：判断两点之间是否存在只经过空格的四连通通路，并记录路径
  输入参数：const int grid[MAX_SIZE][MAX_SIZE]：内部数组
		   const int line  ：行数
		   const int column：列数
		   const int fromx、fromy：起点
		   const int tox、toy    ：终点
		   int* step_mem：路径记录数组，找到时依次存放各步位置、以 -1 结束
  返 回 值：true 表示找到通路，false 表示没有通路
  说    明：只做上下左右四个方向的移动，斜线不算连通
***************************************************************************/
bool Grid_find_path(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int fromx, const int fromy, const int tox, const int toy, int* step_mem)
{
	bool visited[MAX_SIZE][MAX_SIZE] = { 0 };
	if (Grid_dfs(fromx, fromy, tox, toy, grid, visited, line, column, step_mem)) {
		return 1;
	}
	else {
		return 0;
	}
}

//计算得分

/***************************************************************************
  函数名称：Grid_calc_score
  功    能：找出所有横向、纵向、斜向连续 5 个及以上的同色球，消去并计算本次得分
  输入参数：int grid[MAX_SIZE][MAX_SIZE]：内部数组（被消去的格子会置 0）
		   const int line  ：行数
		   const int column：列数
		   int *color_cnt   ：各颜色被消去的个数统计，传 NULL 表示不需要统计
  返 回 值：本次得分，(tot-1)*(tot-2)，其中 tot 是被消去的总球数；tot<5 时为 0
  说    明：四个方向（右、下、右下、左下）各查一次，被消去的球所在格置 0
***************************************************************************/
int Grid_calc_score(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int *color_cnt)
{
	const int DIRECTION[4][2] = { {0, 1}, {1, 0},{1, 1}, {1, -1} }; // 右 下 右下 左下
	bool visited[100][4] = { 0 };
	int tot = 0;
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < column; j++) {
			int pos = Calc_pos(i, j);
			for (int dir = 0; dir < 4; dir++) {
				if (visited[pos][dir] || !grid[i][j]) {
					continue;
				}
				int cnt = 0;
				int x = i, y = j;
				while (In_grid(x, y, line, column) && grid[x][y] == grid[i][j]) {
					cnt++;
					x += DIRECTION[dir][0], y += DIRECTION[dir][1];
				}
				if (cnt >= 5) {
					int x = i, y = j;
					for (int k = 0; k < cnt; k++) {
						visited[Calc_pos(x, y)][dir] = 1;
						x += DIRECTION[dir][0], y += DIRECTION[dir][1];
					}
				}
			}
			bool flag = 0;
			for (int dir = 0; dir < 4; dir++) {
				if (visited[pos][dir]) {
					flag = 1;
					tot++;
					if (color_cnt != NULL) {
						color_cnt[grid[i][j]]++;
					}
				}
			}
			if (flag) {
				grid[i][j] = 0;
			}
		}
	}
	if (tot < 5) {
		return 0;
	}
	return (tot - 1) * (tot - 2);
}

// 单次任务结束

/***************************************************************************
  函数名称：End_of_case
  功    能：显示"本小题结束，请输入End继续..."，读入 END 后才返回
  输入参数：无
  返 回 值：无
  说    明：输入错误时提示并重新输入，保证用户看清结果后再进入下一小题
***************************************************************************/
void End_of_case()
{
	int X, Y;
	cct_getxy(X, Y);
	while (true) {
		cct_gotoxy(X, Y);
		cout << setw(100) << ' ' << '\r';
		cout << "本小题结束，请输入End继续...";
		char str[10] = "", * p = str - 1;
		while ((*(++p) = (char)getchar()) != '\n') {
			*p = Menu_toupper(*p);
		}
		*p = '\0';
		if (strcmp(str, "END")){
			cout << "输入错误，请重新输入";
		}
		else {
			break;
		}
	}
}


