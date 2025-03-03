#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include<mmsystem.h>

typedef struct
{
    int x;
    int y;
}dot;
struct D_snake {
    int length;                 //长度（不包括头）
    dot body[1001];             //身体位置
    int direction[1001];        //方向
    int speed;                  //速度
    int body_way[1001];         //剩余要走路程
    int turn[1001];
}snake;

const clock_t FPS = 1000 / 60;  //限制帧率为60fps
const char dct[4][20] = {"→", "←", "↑", "↓"};   //存储方向字符
short cd = 0;                     //判断能否转向
short window_on = 0;              //判断是否存在窗口
dot gold;                           //金币坐标
short gold_exist = 0;             //是否存在金币
short if_menu = 1;                //是否存在菜单      
short end = 0;

//x1,y1,x2,y2
int line_map[1001][4] = {
    {100, 500, 300, 500},
    {100, 300, 100, 500},
    {500, 100, 700, 100},
    {700, 100, 700, 300},
    {300, 100, 300, 200},
    {200, 200, 300, 200},
    
};
const short line_num = 6;


//画外边框
void Draw_border() {
    cleardevice();

    //外边框为实线白色
    setlinestyle(PS_SOLID, 3);
    setlinecolor(WHITE);
    rectangle(10, 10, 790, 590);
    //内边框为虚线红色
    setlinestyle(PS_DASH, 3);
    setlinecolor(RED);
    rectangle(15, 15, 785, 585);
}

//判断是否死亡
int defeated() {
    //是否超出范围
    if (snake.body[0].x <= 15 || snake.body[0].x >= 785 || snake.body[0].y <= 15 || snake.body[0].y > 585) {
        return 1;
    }
    //是否碰撞身体
    else if (getpixel(snake.body[0].x, snake.body[0].y) == WHITE) {
        return 1;
    }
    return 0;
}

//绘图
void Display() {
    //设置背景色与着色方式为黑色透明
    setbkcolor(BLACK);
    setbkmode(TRANSPARENT);
    cleardevice();
    
    //画边框
    Draw_border();
    
    //画蛇头
    setfillcolor(RED);
    solidcircle(snake.body[0].x, snake.body[0].y, 5);

    //画蛇身
    setfillcolor(WHITE);
    for (int i = 1; i <= snake.length; i++) {
        solidcircle(snake.body[i].x, snake.body[i].y, 5);
    }

    //画墙壁
    if(gold_exist){
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 3);
        for (int i = 0; i < line_num; i++) {
            line(line_map[i][0], line_map[i][1], line_map[i][2], line_map[i][3]);
        }
    }

    //画金币
    if (gold_exist) {
        setfillcolor(YELLOW);
        solidcircle(gold.x, gold.y, 10);
    }

}

//计算蛇身体坐标前移一格
void calculate_position() {
    //计算蛇头
    switch (snake.direction[0])
    {
    case UP:
        snake.body[0].y -= snake.speed;
        break;
    case DOWN:
        snake.body[0].y += snake.speed;
        break;
    case RIGHT:
        snake.body[0].x += snake.speed;
        break;
    case LEFT:
        snake.body[0].x -= snake.speed;
        break;
    default:
        break;
    }
    //依次计算身体
    for (int i = snake.length; i >= 1; i--) {
        switch (snake.direction[i])
        {
        case UP:
            snake.body[i].y -= snake.speed;
            break;
        case DOWN:
            snake.body[i].y += snake.speed;
            break;
        case RIGHT:
            snake.body[i].x += snake.speed;
            break;
        case LEFT:
            snake.body[i].x -= snake.speed;
            break;
        default:
            break;
        }
        //如果前面一个身体转弯了，记录前一个身体位置
        if (snake.direction[i] != snake.direction[i - 1]) {
            if (snake.body_way[i] != 0) {
                snake.body_way[i]--;
            }
            //还没到转点
            else if (snake.turn[i] == 0) {
                if (snake.speed == 1) {
                    snake.body_way[i] = 8;
                }
                else if (snake.speed == 2) {
                    snake.body_way[i] = 3;
                }
                snake.turn[i] = 1;
            }
            //到达转点位置
            else {
                snake.turn[i] = 0;
                snake.direction[i] = snake.direction[i - 1];
            }
        }
    }
}

//整合移动、显示函数
void Move() {
    calculate_position();
    Display();
}

//生成身体
void Summon_body() {
    snake.length++; //长度++
    snake.direction[snake.length] = snake.direction[snake.length - 1];
    //按方向计算
    switch (snake.direction[snake.length - 1])
    {
    case UP:
        snake.body[snake.length].x = snake.body[snake.length - 1].x;
        snake.body[snake.length].y = snake.body[snake.length - 1].y + 10;
        break;
    case DOWN:
        snake.body[snake.length].x = snake.body[snake.length - 1].x;
        snake.body[snake.length].y = snake.body[snake.length - 1].y - 10;
        break;
    case RIGHT:
        snake.body[snake.length].x = snake.body[snake.length - 1].x - 10;
        snake.body[snake.length].y = snake.body[snake.length - 1].y;
        break;
    case LEFT:
        snake.body[snake.length].x = snake.body[snake.length - 1].x + 10;
        snake.body[snake.length].y = snake.body[snake.length - 1].y;
        break;
    default:
        break;
    }
    //蛇长达5后加速
    if (snake.length == 5) {
        snake.speed = 2;
    }
    
}

//显示菜单
void Draw_menu() {
    cleardevice();
    int alpha = 0, d = 1;
    settextstyle(96, 0, NULL);
    snake.body[0].x = 400; snake.body[0].y = 450;
    //生成10段身体
    for (int i = 1; i <= 10; i++) {
        Summon_body();
    }
    //蛇循环移动直至按下Enter键
    while (true) {
        alpha += d * 5;
        //达到最亮后开始变暗
        if (alpha == 255) {
            d = -1;
        }
        //达到最暗后开始变亮
        else if (alpha == 0) {
            d = 1;
        }
        //到达边界后转弯
        if(snake.direction[0] == LEFT && snake.body[0].x <= 150) {
            snake.direction[0] = UP;
        }
        else if(snake.direction[0] == UP && snake.body[0].y <= 150) {
            snake.direction[0] = RIGHT;
        }
        else if(snake.direction[0] == RIGHT && snake.body[0].x >= 650) {
            snake.direction[0] = DOWN;
        }
        else if(snake.direction[0] == DOWN && snake.body[0].y >= 450) {
            snake.direction[0] = LEFT;
        }
        //绘图
        BeginBatchDraw();   //双缓冲防止闪屏
        Move();
        settextstyle(96, 0, NULL);
        settextcolor(WHITE);
        outtextxy(120, 180, "Gluttonous Snake");
        settextstyle(36, 0, NULL);
        outtextxy(320, 300, "贪  吃  蛇");
        settextstyle(18, 0, NULL);
        settextcolor(RGB(alpha, alpha, alpha));
        outtextxy(321, 350, "按下Enter键以开始");
        outtextxy(325, 380, "按下ESC键以退出");
        settextcolor(WHITE);
        settextstyle(12, 0, NULL);
        outtextxy(638, 560, "powered by--FOLINK");
        EndBatchDraw();
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            cleardevice();
            memset(&snake, 0, sizeof(snake));
            //初始化贪吃蛇头的位置、朝向
            snake.direction[0] = rand() % 4;
            snake.body[0].x = 400;
            snake.body[0].y = 300;
            snake.body[0].x = 400;
            snake.body[0].y = 300;
            snake.length = 0;
            snake.speed = 2;

            //Display();
            return;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            end = 1;
            break;
        }
        Sleep(10);
    }
}

//检测金币生成是否与墙体、蛇体碰撞
int detect_colide(int x, int y) {
    for (int i = -10; i <= 10; i++) {
        if (getpixel(x + i, y) == WHITE) {
            return 1;
        }
    }
    for (int i = -10; i <= 10; i++) {
        if (getpixel(x, y + i) == WHITE) {
            return 1;
        }
    }
    return 0;
}

//生成金币
void Summon_gold() {
    while (1) {
        gold.x = rand() % 741 + 30;
        gold.y = rand() % 541 + 30;
        if (!detect_colide(gold.x, gold.y)) {
            break;
        }
    }
    gold_exist = 1;
}

//初始化
void INIT() {
    //防止窗口多次初始化
    if (!window_on) {
        initgraph(800, 600);
        window_on = 1;
    }
    memset(&snake, 0, sizeof(snake));
    //初始化贪吃蛇头的位置、朝向
    snake.direction[0] = rand() % 4;
    snake.body[0].x = 400;
    snake.body[0].y = 300;
    snake.body[0].x = 400;
    snake.body[0].y = 300;
    snake.length = 0;
    snake.speed = 2;
    gold_exist = 0;

    Display();
}

char str_frame[30], str_score[30], str_dct[30], str_tottime[30];
LOGFONT f;

void Restart() {
    settextstyle(18, 0, NULL);
    for (int i = 255; i >= 0; i--) {
        BeginBatchDraw();
        setfillcolor(RGB(i, 0, 0));
        cleardevice();
        fillcircle(snake.body[0].x, snake.body[0].y, 5);
        Draw_border();

        //画蛇头
        solidcircle(snake.body[0].x, snake.body[0].y, 5);

        //画金币
        setfillcolor(RGB(i, i, 0));
        solidcircle(gold.x, gold.y, 10);

        settextcolor(RGB(i, i, i));
        if (cd) {
            outtextxy(600, 30, "Turing Status:  Loading...");
        }
        else {
            outtextxy(600, 30, "Turing Status:      Ready!");
        }
        outtextxy(30, 30, str_frame);
        outtextxy(30, 50, str_score);
        outtextxy(650, 50, str_dct);
        EndBatchDraw();
    }
}

void Start_Instruction() {
    settextcolor(WHITE);
    cleardevice();
    int h = f.lfHeight;
    for (int i = 1; i <= 255; i++) {
        BeginBatchDraw();
        settextcolor(RGB(i, i, i));
        settextstyle(48, 0, NULL);
        outtextxy(300, 350, "Start in 3s");
        settextstyle(96, 0, NULL);
        outtextxy(120, 180, "Gluttonous Snake");
        EndBatchDraw();
        //Sleep时间不准……只能通过这样的方式延时了……
        for (int i = 1; i <= 10000000; i++) {

        }
    }
    cleardevice();
    BeginBatchDraw();
    settextstyle(48, 0, NULL);
    outtextxy(300, 350, "Start in 2s");
    settextstyle(96, 0, NULL);
    outtextxy(120, 180, "Gluttonous Snake");
    EndBatchDraw();
    Sleep(1000);
    cleardevice();
    BeginBatchDraw();
    settextstyle(48, 0, NULL);
    outtextxy(300, 350, "Start in 1s");
    settextstyle(96, 0, NULL);
    outtextxy(120, 180, "Gluttonous Snake");
    EndBatchDraw();
    Sleep(500);
    cleardevice();
    for (int i = 255; i >= 0; i-=5) {
        BeginBatchDraw();
        settextcolor(RGB(i, i, i));
        settextstyle(48, 0, NULL);
        outtextxy(300, 350, "Start in 1s");
        settextstyle(96, 0, NULL);
        outtextxy(120, 180, "Gluttonous Snake");
        EndBatchDraw();
        Sleep(1);
        cleardevice();
    }
    settextstyle(18, 0, NULL);
}
    
int main() {
    //播放音乐
    mciSendString("open pixel_rebelz.mp3", NULL, 0, NULL);
    mciSendString("play pixel_rebelz.mp3", NULL, 0, NULL);

    INIT();
    if (if_menu) {
        Draw_menu();
    }
    if (end) {
        closegraph();
        return 0;
    }
    if_menu = 1;
    Summon_gold();
    ExMessage msg;
    Start_Instruction();
    gettextstyle(&f);
    int i = 0, op_time = clock();
    Display();
    Sleep(500);
    while (true) {
        f.lfHeight = 16;
        settextstyle(&f);
        int st_time = clock();
        i++;
        sprintf_s(str_frame, "Frame:%d", i);
        sprintf_s(str_score, "分数:%d", snake.length);
        sprintf_s(str_dct, "当前朝向:");
        if (clock() - op_time >= 500) {
            cd = 0;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            if (snake.direction[0] != DOWN && snake.direction[0] != UP) {
                if (!cd) {
                    snake.direction[0] = UP;
                    op_time = clock();
                    cd = 1;
                }
            }
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            if (snake.direction[0] != UP && snake.direction[0] != DOWN) {
                if (!cd) {
                    snake.direction[0] = DOWN;
                    op_time = clock();
                    cd = 1;
                }
            }
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            if (snake.direction[0] != RIGHT && snake.direction[0] != LEFT) {
                if (!cd) {
                    snake.direction[0] = LEFT;
                    op_time = clock();
                    cd = 1;
                }
            }
        }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            if (snake.direction[0] != LEFT && snake.direction[0] != RIGHT) {
                if (!cd) {
                    snake.direction[0] = RIGHT;
                    op_time = clock();
                    cd = 1;
                }
            }
        }
        else if (GetAsyncKeyState('R') & 0x8000) {
            outtextxy(snake.body[0].x - 80, snake.body[0].y + 50, "再次按下Enter键重新开始");
            Sleep(500);
            flushmessage();
            getmessage(&msg, EX_KEY);
            if (msg.ch == VK_RETURN) {
                if_menu = 0;
                Restart();
                main();
            }
        }
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            outtextxy(snake.body[0].x - 80, snake.body[0].y + 50, "再次按下Enter键以返回菜单");
            Sleep(500);
            flushmessage();
            getmessage(&msg, EX_KEY);
            if (msg.ch == VK_RETURN) {
                Restart();

                main();
            }
        }

        //调试指令
        //else if (GetAsyncKeyState(VK_F4) & 0x8000) {
        //    Summon_body();
        //}
        //if (GetAsyncKeyState(VK_F1)) {
        //    snake.speed = 2;
        //}
        //if (GetAsyncKeyState(VK_F2)) {
        //    snake.speed = 1;
        //}
        //if (GetAsyncKeyState(VK_F3) & 0x8000) {
        //    break;
        //}

        settextcolor(RGB(254, 254, 254));
        BeginBatchDraw();
        Move();
        if (getpixel(snake.body[0].x, snake.body[0].y) == YELLOW) {
            for(int k = 1; k <= 5; k++){
                Summon_body();
            }
            Summon_gold();
        }
        if (cd) {
            outtextxy(600, 30, "转向状态:Loading...");
        }
        else {
            outtextxy(600, 30, "转向状态:Ready!");
        }
        outtextxy(30, 30, str_frame);
        outtextxy(30, 50, str_score);
        outtextxy(600, 70, str_dct);
        outtextxy(650, 520, "按下ESC键退出");
        outtextxy(650, 550, "按下R键重新开始");
        int h = f.lfHeight;
        f.lfHeight = 36;
        settextstyle(&f);
        outtextxy(670, 60, dct[snake.direction[0]]);
        EndBatchDraw();
        f.lfHeight = h;
        settextstyle(&f);

        if (defeated()) {
            break;
        }

        int dr_time = clock() - st_time;
        if (dr_time < FPS) {
            Sleep(FPS - dr_time - 1);
        }
    }
    sprintf_s(str_tottime, "存活时间:%5.1f seconds", (float)i / 60);
    f.lfHeight = 96;
    settextstyle(&f);
    outtextxy(190, 230, "GAME OVER!");
    outtextxy(330, 120, "X_X");
    f.lfHeight = 36;
    settextstyle(&f);
    outtextxy(340, 360, str_score);
    outtextxy(250, 420, str_tottime);
    settextstyle(12, 0, NULL);
    outtextxy(600, 400, "按下R键返回主菜单");
    outtextxy(605, 450, "按下ESC键退出");
    while (1) {
        getmessage(&msg, EX_KEY);
        if (msg.ch == 'R') {
            Restart();
            flushmessage();
            main();
        }
        else if (msg.ch == VK_ESCAPE) {
            break;
        }
    }
    closegraph();
    return 0;
}