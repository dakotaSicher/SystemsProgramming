//Dakota Sicher
//CS43203
//Pong assignment

#include    <curses.h>
#include    <signal.h>
#include    <stdlib.h>
#include    <sys/time.h>
#include    <sys/socket.h>
#include    <unistd.h>
#include    <stdio.h>
#include    <time.h>
#include    "pong.h"

void on_tick(int);
void set_up();
void wrap_up();
void draw_court();
int  set_ticker(int);
void update_score();
void print_score();

int sock_fd;
char myName[20];
struct ppball the_ball;
struct paddle the_paddle;
int netSide;
int paddleSide;
char opponent[20];

void wait_state();
void play_state();
//void do_miss();
//void do_quit();

int main(int ac, char *av[])
{
    srand(time(NULL));
    atexit(wrap_up);

    int sock_id;
    // FILE *fpin, *fpout;
    int isServer = 0;
//determine if server or client by # of args 
    if(ac == 2){
        isServer = 1;
        if( (sock_id = make_server_socket(atoi(av[1]))) == -1){
            perror("socket");
            exit(2);
        }
        printf("waiting for player to connect...\n");
        sock_fd = accept(sock_id, NULL, NULL);
        printf("connected\n");
        if(sock_fd == -1){ 
            perror("connection");
            exit(2);
        }
    }else if(ac == 3){
        if( (sock_fd = connect_to_server(av[1],atoi(av[2]))) == -1){
            perror("connection");
            exit(2);
        }
    }else{
        printf("usage:\n host: ./netpong PORT \n join: ./netpong HOST PORT\n");
        exit(1);
    }
    printf("\nWhat is you name? ");
    scanf("%s",myName);
    // fpin = fdopen(sock_fd,"r");
    // fpout= fdopen(sock_fd,"w");
    
    if(isServer){
        heloMsg(sock_fd,myName);
        if(getMsg(sock_fd,&gameState,&the_ball) < 0){
            perror("message error, unable to complete introduction.");
            exit(1);
        }
        servMsg(sock_fd,balls_left);
        netSide = LEFT_EDGE;
        paddleSide = RIGHT_EDGE;
        gameState = WAIT;
    }
    else{
        if(getMsg(sock_fd,&gameState,&the_ball) < 0){
            perror("message error, unable to complete introduction.");
            exit(1);
        } // getting HELO sets tps and netLen
        //printf("TPS: %d\n Net Length: %d\n",tps,netLen);
        nameMsg(sock_fd,myName);
        if(getMsg(sock_fd,&gameState,&the_ball) == SERV){// getting SERV sets Balls left
            gameState = PLAY;
            //swap net and paddle sides for client
            netSide = RIGHT_EDGE;
            paddleSide = LEFT_EDGE;
            serve(&the_ball);
        }else {
            perror("message error, unable to complete introduction.");
            exit(1);
        }
    }
    set_up();
    while(gameState != DONE_S && gameState != QUIT_S){
        if(gameState == PLAY)
            play_state();
        else if(gameState == WAIT)
            wait_state();
    }

    wrap_up();
}

void play_state(){
    
    signal( SIGALRM, on_tick );
    set_ticker( 1000 / tps ); /* send millisecs per tick */ 

    int c;
    while (gameState == PLAY && ( c = getch()) != 'q' ){
        if ( c == 'j' ){
            paddle_up(&the_paddle);
            //bounce_or_lose(&the_ball,&the_paddle);
        }
        else if ( c == 'k' ){
            paddle_down(&the_paddle);
            //bounce_or_lose(&the_ball,&the_paddle);
        }
    }
    if(c == 'q'){
        quitMsg(sock_fd);
        gameState = DONE_S;
    }
    signal( SIGALRM , SIG_IGN );
}

void wait_state(){
    int status = getMsg(sock_fd, &gameState,&the_ball);
    switch(status){
        case -1: 
            perror("huh");
            exit(1);
            break;
        case BALL:
            gameState = PLAY;
            the_ball.x_pos = (netSide == LEFT_EDGE ? LEFT_EDGE + 1 : RIGHT_EDGE - 1);
            the_ball.x_dir = (netSide == LEFT_EDGE ? 1 : -1);
            break;
        case MISS:
            ++myScore;
            --balls_left;
            update_score();
            serve(&the_ball);
            gameState = PLAY;
            break;
        case QUIT:
            gameState = QUIT_S;
            break;
        case DONE:
            gameState = DONE_S;
            ++myScore;
            break;
        default:
            break;
    } 
}


void on_tick(int signum)
{
    signal( SIGALRM , SIG_IGN );        /* dont get caught now  */

    if (move_ball(&the_ball)){

        draw_paddle(&the_paddle);

        switch(bounce_or_lose(&the_ball,&the_paddle) ){
            case -1: // miss
                opponentScore++;
                balls_left--;
                update_score();
                if(balls_left > 0){
                    gameState = WAIT;
                    missMsg(sock_fd);
                }else{
                    gameState = DONE_S;
                    doneMsg(sock_fd);
                }
                break;
            case 2: //at the net
                gameState = WAIT;
                ballMsg(sock_fd, &the_ball);
                break;
            default:
                break;
        }

        move(LINES-1,COLS-1);
        refresh();
    }
    if(gameState == PLAY)
        signal( SIGALRM, on_tick);        /* for unreliable systems */
}

//////////////////////////////////////////////////////////////
//Game setup
///////////////////////////////////////////////////////////////


void set_up()
{
    initscr();
    noecho();
    //crmode();
    halfdelay(1);
    //signal( SIGINT , SIG_IGN );
    draw_court();
    paddle_init(&the_paddle);
    refresh();
}



void wrap_up()
{
    close(sock_fd);
    set_ticker( 0 );
    cbreak();
    print_score();
    endwin();       /* put back to normal   */
}

void print_score(){
    clear();
    int row = LINES/2;
    if(gameState == QUIT_S) {
        mvaddstr(row, 10,opponent);
        addstr(" has quite the game.");
    }
    mvaddstr(++row, 10, "The score is: ");
    addstr(myName); addch(' ');
    addch('0' + myScore); addch(' ');
    addstr(opponent); addch(' ');
    addch('0' + opponentScore);
    if(opponentScore != myScore ){
        mvaddstr(LINES/2 + 2, 10, "The Winner is: ");
        char* winner = opponentScore > myScore ? opponent : myName;
        addstr(winner);
    }
    mvaddstr(LINES-1,0,"press anykey to exit");
    refresh();
    getchar();
}

void draw_court(){
    char bound = '-';
    char wall = '|';
    char net = '.';
    int row=TOP_ROW-2;
    move(row,LEFT_EDGE-1);
    addstr(myName);
    addstr(": ");
    addch('0'+ myScore); addch(' ');
    addstr(opponent);
    addstr(": ");
    addch('0'+ opponentScore);
    move(++row,LEFT_EDGE-1);
    for(int i = LEFT_EDGE-1;i<=RIGHT_EDGE;++i)
        addch(bound);
    while(row++ < (TOP_ROW + netLen) ){
        mvaddch(row,netSide,net);
    }
    move(row,LEFT_EDGE-1);
    for(int i = LEFT_EDGE-1;i<=RIGHT_EDGE;++i)
        addch(bound);
}

void update_score(){
    move(TOP_ROW-2,LEFT_EDGE-1);
    addstr(myName);
    addstr(": ");
    addch('0'+ myScore); addch(' ');
    addstr(opponent);
    addstr(": ");
    addch('0'+ opponentScore);
}

//////////////////////////////////////////////////
//Timer contron
//////////////////////////////////////////////////

int set_ticker(int n_msecs )
{
        struct itimerval new_timeset;
        long    n_sec, n_usecs;

        n_sec = n_msecs / 1000 ;
        n_usecs = ( n_msecs % 1000 ) * 1000L ;

        new_timeset.it_interval.tv_sec  = n_sec;        /* set reload  */
        new_timeset.it_interval.tv_usec = n_usecs;      /* new ticker value */
        new_timeset.it_value.tv_sec     = n_sec  ;      /* store this   */
        new_timeset.it_value.tv_usec    = n_usecs ;     /* and this     */

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
