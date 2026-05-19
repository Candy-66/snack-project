#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>

#define U 1
#define D 2
#define L 3
#define R 4

typedef struct SNAKE
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

int score = 0, add = 10;
int status, sleeptime = 150;
snake* head, * food;
snake* q;
int endgamestatus = 0;

char username[50] = {0};
char password[50] = {0};
int userId = 1;
time_t startTime;

// 精美配色
#define BG_COLOR     7    // 游戏背景浅灰
#define WALL_COLOR   9    // 围墙蓝色
#define SNAKE_COLOR  10   // 蛇身亮绿
#define FOOD_COLOR   12   // 食物红色
#define TEXT_COLOR   15   // 纯白文字
#define INFO_COLOR   11   // 提示浅青

void Pos(int x, int y);
void SetColor(int c);
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pauseGame();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();
void HideCursor();

int RegisterUser();
int LoginUser();
void SaveGameLog();
void ShowGameLog();

// 光标定位
void Pos(int x, int y)
{
    COORD pos;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(hOutput, pos);
}

// 设置颜色
void SetColor(int c)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, c);
}

// 隐藏光标
void HideCursor()
{
    CONSOLE_CURSOR_INFO cci;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hOut, &cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cci);
}

// 【极致美化游戏界面】经典贪吃蛇布局
void creatMap()
{
    int i;
    system("cls");
    SetColor(TEXT_COLOR);

    // 顶部游戏大标题
    Pos(22, 0);
    printf("════════ 经典贪吃蛇 ════════");
    
    // 顶部玩家信息栏
    Pos(0, 1);
    SetColor(INFO_COLOR);
    printf("玩家：%s", username);
    Pos(28, 1);
    printf("移动速度：%d ms", sleeptime);
    Pos(55, 1);
    printf("单食物分值：%d", add);

    // 外层粗边框 贴近原版游戏
    SetColor(WALL_COLOR);
    // 上边框
    for(i = 2; i <= 54; i++)
    {
        Pos(i, 3); printf("═");
    }
    // 下边框
    for(i = 2; i <= 54; i++)
    {
        Pos(i, 26); printf("═");
    }
    // 左右竖边框
    for(i = 4; i <= 25; i++)
    {
        Pos(2, i); printf("║");
        Pos(54, i); printf("║");
    }

    // 右侧独立游戏控制面板
    SetColor(INFO_COLOR);
    Pos(60, 4);  printf("┌───────────────┐");
    Pos(60, 5);  printf("│    操作指南    │");
    Pos(60, 6);  printf("├───────────────┤");
    Pos(60, 7);  printf("│ ↑↓←→ 移动     │");
    Pos(60, 8);  printf("│ A键 加速      │");  // 改这里
    Pos(60, 9);  printf("│ S键 减速      │");  // 改这里
    Pos(60,10);  printf("│ 空格 暂停游戏  │");
    Pos(60,11);  printf("│ ESC 退出游戏   │");
    Pos(60,12);  printf("│ L键 查看记录   │");
    Pos(60,13);  printf("└───────────────┘");

    // 分数面板
    Pos(60,15);
    printf("┌───────────────┐");
    Pos(60,16);
    printf("│ 当前分数：%04d │",score);
    Pos(60,17);
    printf("└───────────────┘");

    // 恢复默认字体颜色
    SetColor(TEXT_COLOR);
}

// 初始化蛇
void initsnake()
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));
    tail->x = 26;
    tail->y = 14;
    tail->next = NULL;

    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 26 + 2 * i;
        head->y = 14;
        tail = head;
    }

    SetColor(SNAKE_COLOR);
    q = head;
    while (q != NULL)
    {
        Pos(q->x, q->y);
        printf("●");
        q = q->next;
    }
}

// 判断自撞
int biteself()
{
    snake* self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
            return 1;
        self = self->next;
    }
    return 0;
}

// 生成食物 优化算法
void createfood()
{
    food = (snake*)malloc(sizeof(snake));
    int flag;
    while (1)
    {
        flag = 0;
        food->x = (rand() % 24) * 2 + 4;
        food->y = rand() % 20 + 5;
        snake* p = head;
        while (p != NULL)
        {
            if (p->x == food->x && p->y == food->y)
            {
                flag = 1;
                break;
            }
            p = p->next;
        }
        if (!flag) break;
    }
    SetColor(FOOD_COLOR);
    Pos(food->x, food->y);
    printf("◆");
}

// 撞墙判定
void cantcrosswall()
{
    if (head->x <= 2 || head->x >= 54 || head->y <= 3 || head->y >= 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

// 蛇移动核心优化算法
void snakemove()
{
    cantcrosswall();
    snake* newhead = (snake*)malloc(sizeof(snake));

    switch (status)
    {
        case U: newhead->x = head->x; newhead->y = head->y - 1; break;
        case D: newhead->x = head->x; newhead->y = head->y + 1; break;
        case L: newhead->x = head->x - 2; newhead->y = head->y; break;
        case R: newhead->x = head->x + 2; newhead->y = head->y; break;
        default: newhead->x = head->x; newhead->y = head->y;
    }

    SetColor(SNAKE_COLOR);
    Pos(newhead->x, newhead->y);
    printf("●");
    newhead->next = head;
    head = newhead;

    // 吃到食物
    if (head->x == food->x && head->y == food->y)
    {
        score += add;
        SetColor(INFO_COLOR);
        Pos(69,16);
        printf("%04d",score);
        free(food);
        createfood();
    }
    // 没吃到食物清除尾部
    else
    {
        snake* last = head;
        snake* prev = NULL;
        while (last->next != NULL)
        {
            prev = last;
            last = last->next;
        }
        SetColor(BG_COLOR);
        Pos(last->x, last->y);
        printf(" ");
        free(last);
        prev->next = NULL;
    }

    if (biteself())
    {
        endgamestatus = 2;
        endgame();
    }
}

// 暂停界面美化
void pauseGame()
{
    SetColor(FOOD_COLOR);
    Pos(18, 14);
    printf("===== 游戏暂停 ======");
    SetColor(TEXT_COLOR);
    Pos(18,16);
    printf("按下空格键继续游戏");
    while (1)
    {
        if (_kbhit())
        {
            int key = _getch();
            if (key == 32)
            {
                creatMap();
                // 重绘蛇和食物
                q = head;
                SetColor(SNAKE_COLOR);
                while(q)
                {
                    Pos(q->x,q->y);printf("●");
                    q=q->next;
                }
                SetColor(FOOD_COLOR);
                Pos(food->x,food->y);printf("◆");
                break;
            }
        }
        Sleep(30);
    }
}

// 游戏主循环
// 游戏主循环（已修改：A加速 S减速）
void gamecircle()
{
    status = R;
    while (1)
    {
        if (_kbhit())
        {
            int key = _getch();
            if (key == 0xE0)
            {
                key = _getch();
                switch (key)
                {
                    case 0x48: if (status != D) status = U; break;
                    case 0x50: if (status != U) status = D; break;
                    case 0x4B: if (status != R) status = L; break;
                    case 0x4D: if (status != L) status = R; break;
                }
            }
            // ================== 这里改成字母键 ==================
            else if (key == 'A' || key == 'a')  // A键 = 加速
            {
                if (sleeptime >= 50) sleeptime -= 30;
            }
            else if (key == 'S' || key == 's')  // S键 = 减速
            {
                if (sleeptime <= 300) sleeptime += 30;
            }
            // =====================================================
            else if (key == 27)
            {
                endgamestatus = 3;
                break;
            }
            else if (key == 32)
            {
                pauseGame();
            }
            else if (key == 'L' || key == 'l')
            {
                ShowGameLog();
                creatMap();
                q = head;
                SetColor(SNAKE_COLOR);
                while (q != NULL)
                {
                    Pos(q->x, q->y);
                    printf("●");
                    q = q->next;
                }
                SetColor(FOOD_COLOR);
                Pos(food->x, food->y);
                printf("◆");
            }
        }
        Sleep(sleeptime);
        snakemove();

        SetColor(INFO_COLOR);
        Pos(28,1);
        printf("移动速度：%d ms    ",sleeptime);
    }
}

// 精美欢迎启动界面
void welcometogame()
{
    system("cls");
    SetColor(SNAKE_COLOR);
    Pos(20,8);printf("╔═════════════════════╗");
    Pos(20,9);printf("║      经典贪吃蛇     ║");
    Pos(20,10);printf("║   界面算法优化版    ║");
    Pos(20,11);printf("╚═════════════════════╝");
    SetColor(TEXT_COLOR);
    Pos(22,14);printf("按下任意键开始游戏");
    _getch();
}

// 高颜值游戏结束界面（修复版）
void endgame()
{
    SaveGameLog();
    system("cls");
    SetColor(FOOD_COLOR);
    Pos(25,8);printf("┌───────────────────┐");
    Pos(25,9);printf("│      游戏结束     │");
    Pos(25,10);printf("├───────────────────┤");
    
    // 修复：把每个分支用大括号 {} 包裹起来
    if(endgamestatus == 1)
    {
        Pos(25,11);printf("│    撞击边界失败   │");
    }
    else if(endgamestatus == 2)
    {
        Pos(25,11);printf("│    吞食自身失败   │");
    }
    else
    {
        Pos(25,11);printf("│    主动退出游戏   │");
    }

    Pos(25,12);printf("│  最终得分：%04d  │",score);
    Pos(25,13);printf("└───────────────────┘");
    SetColor(TEXT_COLOR);
    Pos(28,16);system("pause");
    exit(0);
}

// 游戏初始化
void gamestart()
{
    system("mode con cols=95 lines=30");
    startTime = time(NULL);
    HideCursor();
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}

// 注册
int RegisterUser()
{
    FILE* fp = fopen("user.txt", "w");
    if (!fp) return 0;
    system("cls");
    printf("==== 游戏账号注册 ====\n");
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);
    fprintf(fp, "%d %s %s\n", userId, username, password);
    fclose(fp);
    printf("注册成功！按任意键返回主菜单\n");
    _getch();
    return 1;
}

// 登录
int LoginUser()
{
    FILE* fp = fopen("user.txt", "r");
    char dbUser[50] = {0}, dbPwd[50] = {0};
    int dbId;
    if (!fp || fscanf(fp, "%d %s %s", &dbId, dbUser, dbPwd) != 3)
    {
        fclose(fp);
        printf("暂无已注册账号，请先注册！\n");
        system("pause");
        return 0;
    }
    fclose(fp);
    system("cls");
    printf("==== 贪吃蛇账号登录 ====\n");
    printf("用户名：");
    scanf("%s", username);
    printf("密  码：");
    scanf("%s", password);
    if (!strcmp(username, dbUser) && !strcmp(password, dbPwd))
    {
        printf("登录成功！按任意键开始游戏\n");
        _getch();
        return 1;
    }
    else
    {
        printf("账号密码错误！\n");
        system("pause");
        return 0;
    }
}

// 主菜单（登录/注册选择）
int LoginMenu()
{
    int choice;
    while(1)
    {
        system("cls");
        printf("===== 贪吃蛇游戏主菜单 =====\n");
        printf("1. 登录已有账号\n");
        printf("2. 注册新账号\n");
        printf("3. 退出游戏\n");
        printf("请输入选择(1-3): ");
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1:
                if(LoginUser())
                    return 1;
                break;
            case 2:
                if(RegisterUser())
                    return 1;
                break;
            case 3:
                printf("已退出游戏！\n");
                system("pause");
                exit(0);
            default:
                printf("输入错误，请输入1-3之间的数字！\n");
                system("pause");
        }
    }
}

// 保存游戏记录
void SaveGameLog()
{
    FILE* fp = fopen("game_log.txt", "a");
    if (!fp) return;
    time_t endTime = time(NULL);
    int sec = (int)(endTime - startTime);
    struct tm* t = localtime(&startTime);
    fprintf(fp, "%s | 游玩时长:%d秒 | 得分:%d\n",username,sec,score);
    fclose(fp);
}

// 查看游戏记录
void ShowGameLog()
{
    FILE* fp = fopen("game_log.txt", "r");
    char line[300];
    system("cls");
    printf("========== 个人游戏记录 ==========\n");
    printf("----------------------------------\n");
    if (!fp)
        printf("暂无任何游玩记录\n");
    else
    {
        while(fgets(line,sizeof(line),fp))
            printf("%s",line);
        fclose(fp);
    }
    printf("\n按下任意键返回游戏\n");
    _getch();
}

// 主函数
int main()
{
    SetConsoleCP(936);
    SetConsoleOutputCP(936);
    HideCursor();
    if (!LoginMenu())
        return 0;
    score = 0;
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
