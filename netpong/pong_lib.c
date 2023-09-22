//Dakota Sicher
//CS43203
//Pong assignment

#include    <curses.h>
#include    <stdlib.h>
#include    "pong.h"

extern int netSide;
extern int paddleSide;

// on miss return -1
// at net  return 2
// wall or paddle bounce return 1
int bounce_or_lose(struct ppball *bp,struct paddle* pad )
{
    int return_val = 0 ;

    if ( bp->y_pos == TOP_ROW ){
        bp->y_dir = 1 ; 
        return_val = 1 ;
    } else if ( bp->y_pos == BOT_ROW ){
        bp->y_dir = -1 ;
        return_val = 1;
    }
    if ( bp->x_pos == netSide + (netSide == LEFT_EDGE ? 1 : -1) ){
        return_val = 2 ;
    } else if ( bp->x_pos == paddleSide ){
        if(paddle_contact(pad, bp)){
            bp->x_dir *= -1;
            rand_ball_speed(bp);
            return_val = 1;
        }else{
            return_val = -1;
        }
    }
    draw_paddle(pad);

    return return_val;
}


//////////////////////////////////////////////
//Paddle functions
/////////////////////////////////////////////
void draw_paddle(struct paddle* p){
    for(int i = TOP_ROW; i <= BOT_ROW; ++i){
        if(i < p->top || i > p->bot) 
            mvaddch(i,p->col,BLANK);
        else
            mvaddch(i,p->col,p->sym);
    }
    refresh();
}

void paddle_init(struct paddle* p){
    p->top = netLen/2 - 3 + TOP_ROW;
    p->bot = p->top + 5;
    p->col = paddleSide;
    p->sym = DFL_SYM;

    draw_paddle(p);
}

void paddle_up(struct paddle* p){
    if(p->top == TOP_ROW) return;
    p->top--;
    p->bot--;
    draw_paddle(p);
}

void paddle_down(struct paddle* p){
    if(p->bot == BOT_ROW) return;
    p->top++;
    p->bot++;
    draw_paddle(p);
}

int paddle_contact(struct paddle* p, struct ppball* b){
    if(b->x_pos == p->col && (b->y_pos < p->top || b->y_pos > p->bot)) return 0;
    return 1;
}
//////////////////////////////////////////////////
//Ball functions
/////////////////////////////////////////////////

void ball_reset(struct ppball* ball){
    mvaddch(ball->y_pos,ball->x_pos,BLANK);
    ball->y_pos = netLen/2 + TOP_ROW;
    ball->x_pos = (paddleSide == LEFT_EDGE ? paddleSide + 1 : paddleSide - 1);
    ball->symbol = DFL_SYMBOL ;
}

void serve(struct ppball* ball){
    ball_reset(ball);
    rand_ball_speed(ball);
    if(rand()%2)
        ball->y_dir = 1 ;
    else
        ball->y_dir = -1 ;
    ball->x_dir = (paddleSide == LEFT_EDGE ? 1 : -1);
}

void rand_ball_speed(struct ppball* ball){
    ball->y_ttg = ball->y_ttm =  (rand()%(MAX_Y_TTM - MIN_Y_TTM)) + MIN_Y_TTM;
    ball->x_ttg = ball->x_ttm =  (rand()%(MAX_X_TTM - MIN_X_TTM)) + MIN_X_TTM;
}

int move_ball(struct ppball* ball){

    int y_cur = ball->y_pos ;        /* old spot     */
    int x_cur = ball->x_pos ;
    int moved = 0 ;

    if ( ball->y_ttm > 0 && ball->y_ttg-- == 1 ){
        ball->y_pos += ball->y_dir ;  /* move */
        ball->y_ttg =  ball->y_ttm  ;  /* reset*/
        moved = 1;
    }

    if ( ball->x_ttm > 0 && ball->x_ttg-- == 1 ){
        ball->x_pos += ball->x_dir ;  /* move */
        ball->x_ttg = ball->x_ttm  ;  /* reset*/
        moved = 1;
    }

    if ( moved ){
        mvaddch( y_cur, x_cur, BLANK );
        mvaddch( y_cur, x_cur, BLANK );
        mvaddch( ball->y_pos, ball->x_pos, ball->symbol );
    }

    return moved;
}






