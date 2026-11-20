/* BigHW */
#pragma once

#include "../include/cmd_console_tools.h"

/* ------------------------------------------------------------------------------------------------------

     本文件功能：
	1、放 90-01-b2-color_linez_*.cpp 中共用的宏定义、全局只读变量及全部函数声明
	2、MENU 部分已经提炼为公共函数（common_menu.h），本文件不再声明菜单函数

     本文件要求：
	1、只放宏定义、函数声明及全局只读（const）变量，不放函数定义
	2、不允许把公共头文件拷贝到本项目目录中，一律用相对路径包含

   ------------------------------------------------------------------------------------------------------ */

// 常量

#define MAX_SIZE 9
#define MAX_COLOR 7

const int COLOR_MAP[] = { COLOR_HWHITE, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED, COLOR_PINK, COLOR_YELLOW, COLOR_HGREEN };

// tools中的函数

void to_be_continued(const char* prompt, const int X = 0, const int Y = 22);
void End_of_case();
void Random_init();
int Random_get(int lim = 0x7fffffff);
int Calc_pos(const int x, const int y);
void Calc_xy(const int pos, int &x, int &y);
bool In_grid(const int X, const int Y, const int line, const int column);
bool Grid_filled(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column);
bool Input_check_int(int x, const int l, const int r);
void Grid_generate_balls(int* color, const int n = 3);
bool Grid_add_balls(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int* color, const int n = 3);
bool Grid_find_path(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int fromx, const int fromy, const int tox, const int toy, int* step_mem);
int Grid_calc_score(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int *color_cnt = NULL);

// base中的函数

void Base_init_grid(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int init_balls = 5);
void Base_show_grid(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const bool show_background = 1, const char* str = "当前数组");
void Base_show_buf(int* color);
void Base_get_query(bool flag, const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, int& x, int& y);
bool Base_single_step(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column);

// graph中的函数

void Graph_show_border(const int line, const int column, bool show_inner_border = 1);
void Graph_show_grid(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, bool show_inner_border = 1);
bool Graph_single_step(int grid[MAX_SIZE][MAX_SIZE], const int line, const int column);
void Graph_show_state(const int grid[MAX_SIZE][MAX_SIZE], const int line, const int column, const int* buf, const int* color_cnt, const int score);
