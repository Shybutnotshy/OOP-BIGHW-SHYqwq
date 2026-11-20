/* BigHW */


/* ----------------------------------------------------------------------------------

     本文件功能：
	1、存放被 90-01-b1-hanoi_main.cpp 中根据菜单返回值调用的各菜单项对应的执行函数

     本文件要求：
	1、不允许定义外部全局变量（const及#define不在限制范围内）
	2、允许定义静态全局变量（具体需要的数量不要超过文档显示，全局变量的使用准则是：少用、慎用、能不用尽量不用）
	3、静态局部变量的数量不限制，但使用准则也是：少用、慎用、能不用尽量不用
	4、按需加入系统头文件、自定义头文件、命名空间等

----------------------------------------------------------------------------------- */

/*

  源程序函数采用类似Class的命名规则以便调用：
    Hanoi_...  ---  汉诺塔操作相关
    Stack_...  ---  栈模拟相关
    Screen_... ---  图形化显示相关

*/

#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <conio.h>
using namespace std;

#include "../include/cmd_console_tools.h"
#include "../include/cmd_hdc_tools.h"
#include "90-01-b1-hanoi.h"

static int step_count;
static int hanoi_tower[3][MAX_LAYER], layer_count[3];
static int delay_mod;

//================================================ 栈模拟

/***************************************************************************
  函数名称：Stack_push
  功    能：把编号为 num 的盘子压入 id 号柱子的栈顶
  输入参数：int id ：柱号
		    int num：盘子编号
  返 回 值：无
  说    明：
***************************************************************************/
void Stack_push(int id, int num)
{
    hanoi_tower[id][layer_count[id]++] = num;
    return;
}

/***************************************************************************
  函数名称：Stack_top
  功    能：取得 id 号柱子栈顶的盘子编号，不改变栈的内容
  输入参数：int id：柱号
  返 回 值：栈顶盘子编号；柱子为空时返回 MAX_LAYER+1
  说    明：
***************************************************************************/
int Stack_top(int id)
{
    if (!layer_count[id]) {
        return 11;
    }
    return hanoi_tower[id][layer_count[id] - 1];
}

/***************************************************************************
  函数名称：Stack_pop
  功    能：弹出 id 号柱子栈顶的盘子
  输入参数：int id：柱号
  返 回 值：无
  说    明：
***************************************************************************/
void Stack_pop(int id) 
{
    if (layer_count[id]) {
        layer_count[id]--;
    }
    return;
}

//================================================ end

/***************************************************************************
  函数名称：Screen_delay
  功    能：按设定的速度延时；速度为0时改为等待用户按回车，实现单步演示
  输入参数：无
  返 回 值：无
  说    明：延时值保存在静态全局变量 delay_mod 中，由 Run_hanoi 读入
***************************************************************************/
void Screen_delay()
{
    if (!delay_mod) {
        while (true) {
            int ch = _getwch();
            if (ch == '\n' || ch == '\r')
                break;
        }
        return;
    }
    Sleep(delay_mod);
}

/***************************************************************************
  函数名称：Screen_show_tower
  功    能：画出三个底盘和三个立柱，并在状态行显示本次移动的说明
  输入参数：int n   ：本次移动的盘子数，0 表示不需要显示说明
		    char src：起始柱
		    char dst：目标柱
		    bool delay：是否延时
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_show_tower(int n = 0, char src = '\0', char dst = '\0', bool delay = 1)
{
    hdc_init(HDC_COLOR[0], RGB(0, 0, 0), 8 * 120, 16 * 30);
    hdc_cls();
    cct_gotoxy(0, CCT_Max_Y - 3);
    if (n) {
        cout << "从 " << src << " 移动到 " << dst << "，共 " << n << " 层";
    }
    Sleep(HDC_Init_Delay);

    const int underpan_width = 23 * HDC_Base_Width;
    const int pillar_y = HDC_Start_Y - 12 * HDC_Base_High;
    const int pillar_height = HDC_Start_Y - pillar_y;
    const int base_y = HDC_Start_Y;
    const int color = HDC_COLOR[11];

    for (int i = 0; i < 3; i++) {
        int base_x = HDC_Start_X + i * HDC_Underpan_Distance + i * underpan_width;

        hdc_rectangle(base_x, base_y, underpan_width, HDC_Base_High, color);
        if (delay) {
            Sleep(HDC_Init_Delay);
        }
    }

    for (int i = 0; i < 3; i++) {
        int base_x = HDC_Start_X + i * HDC_Underpan_Distance + i * underpan_width;
        int pillar_x = base_x + (underpan_width - HDC_Base_Width) / 2;

        hdc_rectangle(pillar_x, pillar_y, HDC_Base_Width, pillar_height, color);
        if (delay) {
            Sleep(HDC_Init_Delay);
        }
    }
}

/***************************************************************************
  函数名称：Screen_disk_init
  功    能：按当前各柱子的内容，把所有盘子画到立柱上
  输入参数：bool delay：是否在每画一个盘子后延时
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_disk_init(bool delay = 1)
{
    const int underpan_width = 23 * HDC_Base_Width;
    for (int id = 0; id < 3; id++) {
        int base_x = HDC_Start_X + id * HDC_Underpan_Distance + id * underpan_width;
        int base_y = HDC_Start_Y;

        for (int i = 0; i < layer_count[id]; i++) {
            int disk_num = hanoi_tower[id][i];
            int disk_width = (2 * disk_num + 1) * HDC_Base_Width;
            int disk_height = HDC_Base_High;
            int disk_y = base_y - (i + 1) * disk_height;
            int disk_x = base_x + (underpan_width - disk_width) / 2;

            hdc_rectangle(disk_x, disk_y, disk_width, disk_height, HDC_COLOR[disk_num]);

            if (delay) {
                Sleep(HDC_Init_Delay);
            }
        }
    }
}

/***************************************************************************
  函数名称：Screen_init
  功    能：图形界面的初始化
  输入参数：int n ：盘子层数
		    int src：起始柱的柱号
		    int dst：目标柱的柱号
		    bool is_initial：是否先清屏
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_init(int n, int src, int dst, bool is_initial = 1)
{
    if (is_initial) {
        cct_cls();
    }
    Screen_show_tower(n, (char)(src + 'A'), (char)(dst + 'A'));
    Screen_disk_init();
    return;
}

/***************************************************************************
  函数名称：Screen_move
  功    能：单步图形移动
  输入参数：int src ：起始柱的柱号
		    int dst ：目标柱的柱号
		    bool skip：是否跳过延时
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_move(int src, int dst, bool skip = 0)
{
    const int underpan_width = 23 * HDC_Base_Width;
    const int disk_height = HDC_Base_High;
    const int base_y = HDC_Start_Y;

    int disk_num = Stack_top(dst);
    Stack_pop(dst);

    int base_x_src = HDC_Start_X + src * HDC_Underpan_Distance + src * underpan_width;
    int base_x_dst = HDC_Start_X + dst * HDC_Underpan_Distance + dst * underpan_width;
    int pillar_x_src = base_x_src + (underpan_width - HDC_Base_Width) / 2;
    int pillar_x_dst = base_x_dst + (underpan_width - HDC_Base_Width) / 2;
    const int pillar_y = HDC_Start_Y - 12 * HDC_Base_High;

    int disk_width = (2 * disk_num + 1) * HDC_Base_Width;

    int src_count_before = layer_count[src] + 1;
    int start_y = base_y - src_count_before * disk_height;
    int start_x = base_x_src + (underpan_width - disk_width) / 2;

    int dst_count_before = layer_count[dst]; 
    int target_y = base_y - (dst_count_before + 1) * disk_height;
    int target_x = base_x_dst + (underpan_width - disk_width) / 2;

    int cur_x = start_x, cur_y = start_y;
    int direction = (cur_x < target_x) ? 1 : -1;

    while (cur_y > HDC_Top_Y) {
        cur_y -= HDC_Step_Y;
        if (cur_y < HDC_Top_Y) {
            cur_y = HDC_Top_Y;
        }
        hdc_rectangle(cur_x, cur_y, disk_width, disk_height, HDC_COLOR[disk_num]);
        //恢复背景
        hdc_rectangle(cur_x, cur_y + disk_height, disk_width, HDC_Step_Y, HDC_COLOR[0]);
        //恢复柱子
        hdc_rectangle(pillar_x_src, max(cur_y + disk_height, pillar_y), HDC_Base_Width, HDC_Step_Y, HDC_COLOR[11]);
        if (step_count >= 7 && skip) {
            continue;
        }
        Screen_delay();
    }

    while (cur_x != target_x) {
        cur_x += direction * HDC_Step_X;
        
        if (cur_x < target_x) {
            if (cur_x > target_x) {
                cur_x = target_x;
            }
        }
        else {
            if (cur_x < target_x) {
                cur_x = target_x;
            }
        }

        hdc_rectangle(cur_x, cur_y, disk_width, disk_height, HDC_COLOR[disk_num]);
        //恢复背景
        hdc_rectangle(cur_x - direction * disk_width, cur_y, disk_width, disk_height, HDC_COLOR[0]);
        if (step_count >= 7 && skip) {
            continue;
        }
        Screen_delay();
    }

    while (cur_y < target_y) {
        cur_y += HDC_Step_Y;
        if (cur_y > target_y) {
            cur_y = target_y;
        }
        hdc_rectangle(cur_x, cur_y, disk_width, disk_height, HDC_COLOR[disk_num]);
        //恢复背景
        hdc_rectangle(cur_x, cur_y - HDC_Step_Y, disk_width, HDC_Step_Y, HDC_COLOR[0]);
        //恢复柱子
        if (cur_y > pillar_y) {
            hdc_rectangle(pillar_x_dst, pillar_y, HDC_Base_Width, cur_y - pillar_y, HDC_COLOR[11]);
        }
        if (step_count >= 7 && skip) {
            continue;
        }
        Screen_delay();
    }

    Stack_push(dst, disk_num);
}

/***************************************************************************
  函数名称：Screen_show_list
  功    能：打印横向数组
  输入参数：无
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_show_list() 
{
    for (int id = 0; id < 3; id++) {
        cout << char('A' + id) << ":";
        for (int i = 0; i < 10; i++) {
            if (i < layer_count[id]) {
                cout << setw(2) << hanoi_tower[id][i];
            }
            else {
                cout << "  ";
            }
        }
        cout << " ";
    }
}

/***************************************************************************
  函数名称：Screen_update
  功    能：在(x,y)处更新数组可视化
  输入参数：int n   ：盘子编号
		    char src：起始柱
		    char dst：目标柱
		    const int x、const int y：显示位置
		    bool is_initial：是否是初始状态
  返 回 值：无
  说    明：
***************************************************************************/
void Screen_update(int n, char src, char dst, const int x, const int y, bool is_initial = 0)
{
    cct_gotoxy(x, y);
    if (is_initial) {
        cout << "初始:                 ";
    }
    else {
        cout << "第" << setw(4) << ++step_count << " 步:(" << setw(2) << n << "# " << src << "-->" << dst << ")  ";
    }
    Screen_show_list();

    int x1 = 9, x2 = 43;
    int BASE_Y = y - 3;
    int PILLAR_X[] = { 11, 26, 41 };

    cct_gotoxy(x1, BASE_Y);

    for (int x = x1; x <= x2; x++) {
        cout << '=';
    }

    for (int i = 0; i < 3; i++) {
        cct_gotoxy(PILLAR_X[i], BASE_Y + 1);
        cout << char('A' + i);
    }

    for (int h = 0; h < MAX_LAYER; h++) {
        int y = BASE_Y - h - 1;
        for (int i = 0; i < 3; i++) {
            cct_gotoxy(PILLAR_X[i] - 1, y);

            if (h < layer_count[i]) {
                cout << setw(2) << hanoi_tower[i][h];
            }
            else {
                cout << "  ";
            }
        }
    }

    Screen_delay();
}

/***************************************************************************
  函数名称：Screen_show_step
  功    能：文字演示的步骤详解
  输入参数：int n   ：盘子编号
		    char src：起始柱
		    char dst：目标柱
		    int mod ：菜单选项
  返 回 值：无
  说    明：mod=1/2/3 为文字及横向数组，mod=4/8/9 为字符界面的动画
***************************************************************************/
void Screen_show_step(int n, char src, char dst, int mod)
{
    if (mod == 1) {
        cout << n << "# " << src << "---->" << dst << endl;
    }
    if (mod == 2) {
        cout << "第" << setw(4) << ++step_count << "步(" << setw(2) << n << "# " << src << "---->" << dst << ")" << endl;
    }
    if (mod == 3) {
        cout << "第" << setw(4) << ++step_count << "步(" << setw(2) << n << "# " << src << "---->" << dst << ") ";
        Screen_show_list();
        cout << endl;
    }
    if (mod == 4) {
        Screen_update(n, src, dst, MenuItem4_Start_X, MenuItem4_Start_Y);
    }

    if (mod == 8) {
        Screen_update(n, src, dst, MenuItem8_Start_X, MenuItem8_Start_Y);
    }

    if (mod == 9) {
        Screen_update(n, src, dst, MenuItem9_Start_X, MenuItem9_Start_Y);
    }
}

/***************************************************************************
  函数名称：Hanoi_init
  功    能：初始化数组
  输入参数：int n  ：盘子数
		    int src：起始柱的柱号
  返 回 值：无
  说    明：
***************************************************************************/
void Hanoi_init(int n, int src) 
{
    step_count = 0;
    
    for (int i = 0; i < 3; i++) {
        layer_count[i] = 0;
    }

    for (int i = n; i > 0; i--) {
        Stack_push(src, i);
    }

    return;
}

/***************************************************************************
  函数名称：Hanoi_move
  功    能：把 src 柱顶端的盘子移到 dst 柱，并显示这一步
  输入参数：int n  ：盘子层数
		    int src：起始柱的柱号
		    int dst：目标柱的柱号
		    int mod：菜单选项
  返 回 值：0 表示移动成功，-1 表示非法移动（源柱为空或大盘压小盘）
  说    明：
***************************************************************************/
int Hanoi_move(int n, int src, int dst, int mod) //单步移动
{ 
    int p1 = Stack_top(src), p2 = Stack_top(dst);
    if (!layer_count[src]) {
        cct_gotoxy(MenuItem9_Start_X, MenuItem9_Start_Y + 3);
        cout << setw(100) << ' ' << '\r';
        cout << "源柱为空!";
        return -1;
    }

    if (p1 > p2) {
        cct_gotoxy(MenuItem9_Start_X, MenuItem9_Start_Y + 3);
        cout << setw(100) << ' ' << '\r';
        cout << "大盘压小盘，非法移动!";
        return -1;
    }
    Stack_pop(src);
    Stack_push(dst, p1);
    Screen_show_step(p1, (char)(src + 'A'), (char)(dst + 'A'), mod);

    if (mod >= 7) {
        Screen_move(src, dst, 1);
    }

    return 0;
}

/***************************************************************************
  函数名称：Hanoi_main
  功    能：递归求解汉诺塔
  输入参数：int n  ：本次要移动的层数
		    int src：起始柱的柱号
		    int tmp：中转柱的柱号
		    int dst：目标柱的柱号
		    int mod：菜单选项
  返 回 值：无
  说    明：标准递归：先移 n-1 层到中转柱，再移最大盘，最后把 n-1 层移到目标柱
***************************************************************************/
void Hanoi_main(int n, int src, int tmp, int dst, int mod)
{
    if (n == 1) {
        Hanoi_move(n, src, dst, mod);
        return;
    }
    Hanoi_main(n - 1, src, dst, tmp, mod);
    Hanoi_move(n, src, dst, mod);
    Hanoi_main(n - 1, tmp, src, dst, mod);
}

/***************************************************************************
  函数名称：Hanoi_check
  功    能：是否已经全部移到目标柱
  输入参数：int n ：盘子层数
		    int dst：目标柱的柱号
  返 回 值：true 表示已完成，false 表示未完成
  说    明：
***************************************************************************/
bool Hanoi_check(int n, int dst) //检查是否移动完成
{ 
    return layer_count[dst] == n;
}

/***************************************************************************
  函数名称：To_be_continued
  功    能：清除提示行并显示"按回车键继续..."，等待用户按回车
  输入参数：无
  返 回 值：无
  说    明：
***************************************************************************/
void To_be_continued()
{
    cout << setw(100) << ' ' << '\r';
    cout << "按回车键继续...";

    while (_getch() != '\r')
        ;

    return;
}

/***************************************************************************
  函数名称：Run_hanoi
  功    能：按菜单选项执行演示
  输入参数：int mod：菜单选项
  返 回 值：-1 表示退出，0 表示本题结束
  说    明：
***************************************************************************/
int Run_hanoi(int mod)
{
	cout << mod << endl << endl;

    if (!mod) {
        return -1;
    }

    if (mod == 5) {
        cct_cls();
        Screen_show_tower();
        To_be_continued();
        return 0;
    }

    //输入内容

    int n;
    char src, dst;

    while (true) {
        cout << "请输入汉诺塔的层数(1-10)：" << endl;
        cin >> n;
        cin.clear();
        cin.ignore(10000, '\n');
        if (cin.fail()) {
            continue;
        }
        if (n >= 1 && n <= 10)
            break;

    }

    while (true) {
        cout << "请输入起始柱(A-C)：" << endl;
        cin >> src;
        cin.clear();
        cin.ignore(10000, '\n');
        if (cin.fail()) {
            continue;
        }
        if (src >= 'a' && src <= 'c') {
            src += 'A' - 'a';
        }
        if (src >= 'A' && src <= 'C')
            break;

    }

    while (true) {
        cout << "请输入目标柱(A-C)：" << endl;
        cin >> dst; 
        cin.clear();
        cin.ignore(10000, '\n');
        if (cin.fail()) {
            continue;
        }
        if (dst >= 'a' && dst <= 'c') {
            dst += 'A' - 'a';
        }
        if (dst >= 'A' && dst <= 'C') {
            if (src == dst) {
                cout << "目标柱(" << dst << ")不能与起始柱(" << src << ")相同" << endl;
                continue;
            }
            break;
        }
    }

    if (mod == 4) {
        while (true) {
            cout << "请输入移动速度(0-200：0-按回车单步演示 1-200:延时1-200ms) ";
            cin >> delay_mod;
            cin.clear();
            cin.ignore(10000, '\n');
            if (cin.fail()) {
                continue;
            }

            if (delay_mod >= 0 && delay_mod <= 200)
                break;

        }
    }

    if (mod >= 7) {
        while (true) {
            cout << "请输入移动速度(0-20：0-按回车单步演示 1-20:延时1-20ms) ";
            cin >> delay_mod;
            cin.clear();
            cin.ignore(10000, '\n');
            if (cin.fail()) {
                continue;
            }

            if (delay_mod >= 0 && delay_mod <= 20)
                break;

        }
    }
    
	//初始化

    Hanoi_init(n, src - 'A');

    if (mod >= 4) {
        cct_cls();
        cct_gotoxy(0, CCT_Max_Y - 3);
        cout << "从 " << src << " 移动到 " << dst << "，共 " << n << " 层，延时设置为 " << delay_mod << "ms";
    }

    if (mod == 6) {
        Screen_init(n, src - 'A', dst - 'A');
        cct_gotoxy(CCT_Max_X, CCT_Max_Y - 1);
        To_be_continued();
        return 0;
    }

    if (mod == 7) {
        Screen_init(n, src - 'A', dst - 'A');
        int first_src = src - 'A', first_dst = (n % 2) ? dst - 'A' : 198 - src - dst - 'A';
        Hanoi_move(1, first_src, first_dst, mod);
        cct_gotoxy(CCT_Max_X, CCT_Max_Y - 1);
        To_be_continued();
        return 0;
    }

    if (mod == 8) {
        Screen_update(n, src, dst, MenuItem8_Start_X, MenuItem8_Start_Y, 1);
        Screen_init(n, src - 'A', dst - 'A', 0);
    }

    if (mod == 9) {
        Screen_update(n, src, dst, MenuItem9_Start_X, MenuItem9_Start_Y, 1);
        Screen_init(n, src - 'A', dst - 'A', 0);
        while (true) {
            char frm, gto;
            char str[256];
            int sz = 0;
            cct_gotoxy(MenuItem9_Start_X, MenuItem9_Start_Y + 2);
            cout << setw(100) << ' ' << '\r';
            cout << "请输入移动的柱号(命令形式：AC = A顶端的盘子移动到C，Q = 退出) ：";

            while (true) {
                char tmp = (char)_getwch();
                if (tmp == '\r') {
                    break;
                }

                if (tmp != ' ') {
                    str[sz++] = tmp;
                    cout << tmp;
                }

                if (sz && 'a' <= str[sz - 1] && str[sz - 1] <= 'z') {
                    str[sz - 1] += 'A' - 'a';
                }
            }
            if (sz < 1 || sz > 2) {
                continue;
            }
            if (sz == 1) {
                if (str[0] == 'q' || str[0] == 'Q') {
                    cct_gotoxy(MenuItem9_Start_X, MenuItem9_Start_Y + 3);
                    cout << setw(100) << ' ' << '\r';
                    cout << "游戏中止!!!!!" << endl;
                    To_be_continued();
                    return 0;
                }
            }
            else {
                frm = str[0];
                gto = str[1];
                if ('A' <= frm && frm <= 'C' && 'A' <= gto && gto <= 'C' && frm != gto) {
                    Hanoi_move(n, frm - 'A', gto - 'A', 9);
                }
            }
            if (Hanoi_check(n, dst - 'A')) {
                cct_gotoxy(MenuItem9_Start_X, MenuItem9_Start_Y + 3);
                cout << setw(100) << ' ' << '\r';
                cout << "游戏结束!!!!!" << endl;
                To_be_continued();
                return 0;
            }
        }
        
        return 0;
    }

    //主程序

    char tmp = 198 - src - dst;
    Hanoi_main(n, src - 'A', tmp - 'A', dst - 'A', mod);
    if (mod == 4 || mod >= 6) {
        cct_gotoxy(CCT_Max_X, CCT_Max_Y - 1);
    }
    To_be_continued();
    return 0;
}
