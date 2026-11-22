/* ============================================================
 * 程序名称：骑士巡游（Knight's Tour）
 * 课程名称：2026-2027（1）C语言（通识选修）期末大作业
 * 题目说明：在 8×8 的国际象棋棋盘上，让马从指定的起点出发，
 *           按照“马走日”的规则走遍全部 64 个格子，
 *           每个格子只允许经过一次，输出整条巡游路线。
 * 求解思路：深度优先搜索（回溯）+ Warnsdorff 启发式，
 *           每次优先走向“后续出路最少”的格子，从而大幅减少搜索量。
 * 开发环境：Visual Studio 2022（MSVC），C 语言控制台程序
 * ============================================================ */

#define _CRT_SECURE_NO_WARNINGS     /* 允许使用 scanf，避免 VS 的 C4996 警告 */
#include <stdio.h>
#include <time.h>

#define N 8                 /* 棋盘边长，8×8 共 64 格 */
#define MAX_STEP (N * N)    /* 需要走的总步数：64 */

/* 马可以走的 8 个方向（行、列的增量），(2,1) 就是“马走日” */
static const int DR[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
static const int DC[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

static int board[N][N];                       /* 每格的到达顺序，0 表示没走过 */
static int pathR[MAX_STEP], pathC[MAX_STEP];  /* 每一步所在的行、列（从 0 开始） */
static long nodes;                            /* 搜索过程中访问过的节点数 */

/* 判断 (r, c) 是否落在棋盘之内 */
static int inBoard(int r, int c)
{
    return r >= 0 && r < N && c >= 0 && c < N;
}

/* 计算 (r, c) 这一格按马步还能走向几个没走过的格子，即它的“出路数” */
static int degree(int r, int c)
{
    int k, count = 0;
    for (k = 0; k < 8; k++) {
        int nr = r + DR[k];             /* 尝试第 k 个方向 */
        int nc = c + DC[k];
        if (inBoard(nr, nc) && board[nr][nc] == 0) count++;
    }
    return count;
}

/* 深度优先搜索：第 step 步走到 (r, c)。找到完整路线返回 1，否则返回 0 */
static int solve(int step, int r, int c)
{
    int i, j, n;
    int nr[8], nc[8], d[8];             /* 下一步的候选格子及其出路数 */
    int tr, tc, td;

    board[r][c] = step;                 /* 标记“第 step 步到达这一格” */
    pathR[step - 1] = r;                /* 记录到路线表中 */
    pathC[step - 1] = c;
    nodes++;

    if (step == MAX_STEP) return 1;     /* 64 格全部走完，说明已经找到解 */

    /* 第一步：收集从 (r, c) 出发马能落到的所有未访问格子 */
    n = 0;
    for (i = 0; i < 8; i++) {
        tr = r + DR[i];
        tc = c + DC[i];
        if (inBoard(tr, tc) && board[tr][tc] == 0) {
            nr[n] = tr;
            nc[n] = tc;
            d[n] = degree(tr, tc);      /* 顺便算出这个候选格子的出路数 */
            n++;
        }
    }

    /* 第二步：按出路数从小到大排序（Warnsdorff 规则：先走“死路”） */
    for (i = 1; i < n; i++) {
        tr = nr[i];
        tc = nc[i];
        td = d[i];
        j = i - 1;
        while (j >= 0 && d[j] > td) {   /* 插入排序 */
            nr[j + 1] = nr[j];
            nc[j + 1] = nc[j];
            d[j + 1] = d[j];
            j--;
        }
        nr[j + 1] = tr;
        nc[j + 1] = tc;
        d[j + 1] = td;
    }

    /* 第三步：按排好的顺序逐个尝试，只要有一条路走通就立刻返回 */
    for (i = 0; i < n; i++) {
        if (solve(step + 1, nr[i], nc[i])) return 1;
    }

    board[r][c] = 0;                    /* 这里走不通，撤销标记并回溯 */
    return 0;
}

/* 打印棋盘：每个格子显示“第几步到达”，未走过的格子显示 0 */
static void printBoard(void)
{
    int r, c;

    printf("\n巡游结果（格子中的数字表示第几步到达该格）：\n\n");
    printf("     ");
    for (c = 0; c < N; c++) printf("%3d ", c + 1);   /* 打印列号 */
    printf("\n");

    printf("   +");
    for (c = 0; c < N; c++) printf("----");
    printf("+\n");

    for (r = 0; r < N; r++) {
        printf("%2d |", r + 1);                     /* 打印行号 */
        for (c = 0; c < N; c++) printf("%3d ", board[r][c]);
        printf("|\n");
    }

    printf("   +");
    for (c = 0; c < N; c++) printf("----");
    printf("+\n");
}

/* 按顺序打印每一步的坐标，每行 8 步，便于核对整条路线 */
static void printPath(void)
{
    int i;

    printf("\n每一步的坐标（行,列）：\n");
    for (i = 0; i < MAX_STEP; i++) {
        printf("%2d:(%d,%d)  ", i + 1, pathR[i] + 1, pathC[i] + 1);
        if ((i + 1) % 8 == 0) printf("\n");
    }
}

/* 打印题目说明，让运行界面能自己讲清楚程序在做什么 */
static void printRule(void)
{
    printf("==============================================\n");
    printf("           骑士巡游（Knight's Tour）\n");
    printf("==============================================\n");
    printf("  棋盘：%d×%d（共 %d 格）\n", N, N, MAX_STEP);
    printf("  规则：马从起点出发，按“马走日”移动，\n");
    printf("        每个格子只经过一次，走遍所有格子。\n");
    printf("  算法：深度优先搜索 + Warnsdorff 启发式\n");
    printf("==============================================\n\n");
}

/* 读入起点坐标，输入有效返回 1；越界或格式错误返回 0 */
static int readStart(int *r, int *c)
{
    int rr, cc;

    printf("请输入马的起点（行 列，范围 1~%d，中间用空格隔开）：", N);
    if (scanf("%d %d", &rr, &cc) != 2) return 0;      /* 不是两个整数 */
    if (!inBoard(rr - 1, cc - 1)) return 0;            /* 超出棋盘范围 */

    *r = rr - 1;                                       /* 改成数组下标 */
    *c = cc - 1;
    return 1;
}

/* 主函数：打印说明、读入起点、调用搜索、输出结果和耗时 */
int main(void)
{
    int sr, sc;
    clock_t t0, t1;
    double sec;

    printRule();
    if (!readStart(&sr, &sc)) {
        printf("输入无效：请输入 1~%d 之间的两个整数。\n", N);
        return 1;
    }

    nodes = 0;
    t0 = clock();                                      /* 开始计时 */
    if (!solve(1, sr, sc)) {                           /* 从第 1 步开始搜索 */
        t1 = clock();
        printf("从 (%d,%d) 出发，没有找到能走遍全部 %d 格的路线。\n",
               sr + 1, sc + 1, MAX_STEP);
        printf("搜索节点数：%ld，用时 %.3f 秒。\n",
               nodes, (double)(t1 - t0) / CLOCKS_PER_SEC);
        return 1;
    }
    t1 = clock();                                      /* 结束计时 */
    sec = (double)(t1 - t0) / CLOCKS_PER_SEC;

    printBoard();                                      /* 输出编号棋盘 */
    printPath();                                       /* 输出每一步坐标 */

    printf("\n起点 (%d,%d)，共走 %d 步，搜索节点 %ld 个，用时 %.3f 秒。\n",
           sr + 1, sc + 1, MAX_STEP, nodes, sec);
    return 0;
}
