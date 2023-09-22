//Dakota Sicher
//CS43203
//Pong assignment
#ifndef PONG_H
#define PONG_H


#define BLANK       ' '
#define DFL_SYMBOL  'o'
#define TOP_ROW     5
#define BOT_ROW     20
#define LEFT_EDGE   10
#define RIGHT_EDGE  70
#define X_INIT      10      /* starting col     */
#define Y_INIT      10      /* starting row     */
#define TICKS_PER_SEC   50      /* affects speed    */
#define NUM_BALLS_DFL   3
#define MAX_Y_TTM    8
#define MIN_Y_TTM    4
#define MAX_X_TTM    5
#define MIN_X_TTM    2

struct ppball{
    int x_pos,
        x_ttm,
        x_ttg,
        x_dir,
        y_pos,
        y_ttm,
        y_ttg,
        y_dir;
   char symbol;
};


#define INIT_TOP 10
#define INIT_BOT 15
#define DFL_SYM '#'


struct paddle{
    int top,
        bot,
        col;
   char sym;
};

enum {SERV, BALL, MISS, DONE, QUIT}typedef msg_t;
enum {INTRO, PLAY, WAIT, DONE_S, QUIT_S}typedef state_t;

//globals
static int balls_left = NUM_BALLS_DFL;
static int  tps = TICKS_PER_SEC;
static int netLen = (BOT_ROW-TOP_ROW);
static int myScore = 0;
static int opponentScore = 0;


static state_t gameState = INTRO;

//game funcs
int  bounce_or_lose(struct ppball*,struct paddle*);

void draw_paddle(struct paddle*);
void paddle_init(struct paddle*);
void paddle_up(struct paddle*);
void paddle_down(struct paddle*);
int  paddle_contact(struct paddle*,struct ppball*);

int  move_ball(struct ppball*);
void ball_reset(struct ppball*);
void rand_ball_speed(struct ppball*);
void serve(struct ppball*);


//socklibl.c
int make_server_socket(int);
int make_server_socket_q(int,int);
int connect_to_server(char*,int);

//sppbtp.c
#include <stdio.h>
int heloMsg(int,char*);
int nameMsg(int,char*);
int servMsg(int,int);
int ballMsg(int,struct ppball*);
int missMsg(int);
int quitMsg(int);
int doneMsg(int);
int getMsg(int,state_t*,struct ppball*);

#endif
