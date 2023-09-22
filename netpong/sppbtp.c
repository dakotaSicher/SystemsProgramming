#define VERSION "1.0"

#include "pong.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#define MSGBUF 128
#define ARGLEN 40 

static char msg[MSGBUF];
extern char opponent[20];

int sendMsg(int sock){
    if(send(sock,msg,strlen(msg),0) < 0 ){
        char e[5];
        strncpy(e,msg,4);
        perror(strcat("can't send ",e) );
        return -1;
    }
    else return 0;
}

int heloMsg(int sock, char* name){
    sprintf(msg,"HELO %s %d %d %s\r\n",VERSION,tps,netLen,name);
    return sendMsg(sock);
}

int nameMsg(int sock,char* name){
    sprintf(msg, "NAME %s %s\r\n",VERSION, name);
    return sendMsg(sock);
}

int servMsg(int sock,int numBalls){
    sprintf(msg, "SERV %d\r\n",numBalls);
    return sendMsg(sock);
}

int ballMsg(int sock,struct ppball* ball){
    sprintf(msg, "BALL %d %d %d %d %c\r\n",
	(ball->y_pos - TOP_ROW), 
    ball->x_ttm, ball->y_ttm, ball->y_dir,ball->symbol);
    return sendMsg(sock);
 
}

int missMsg(int sock){
    sprintf(msg,"MISS\r\n");
    return sendMsg(sock);
   
}

int quitMsg(int sock){
    sprintf(msg, "QUIT\r\n");
    return sendMsg(sock);
}

int doneMsg(int sock){
    sprintf(msg,"DONE\r\n");
    return sendMsg(sock);
}

//returns -1 for error in reading expected message from buffer
// return enum type msgType after successfully ready message for SERV and PLAY/WAIT state messages
int getMsg(int sock, state_t* gs,struct ppball* ball){
    char readBuf[MSGBUF];
    memset(readBuf,'\0',MSGBUF);
    if(recv(sock,readBuf,MSGBUF,0) < 0)
        perror("couldn't get message.");
    char msgType[4];

    if(strncmp("QUIT",readBuf,4) == 0){
        return QUIT;
    }
    //intro state
    int status;
    if(*gs == INTRO){
        if(strncmp("HELO",readBuf,4) == 0){
            char vrsn[10];
            status = sscanf(readBuf,"%s %s %d %d %s\r\n",msgType,vrsn,&tps,&netLen,opponent);
            if(status != 5 || status == EOF)
                return -1;
            if (strcmp(vrsn,VERSION) !=0)
                return -1;
            return 0;
        }
        else if(strncmp("NAME",readBuf,4) == 0){
            char vrsn[10];
            status = sscanf(readBuf, "%s %s %s\r\n",msgType,vrsn, opponent);
            if(status != 3 || status == EOF)
                return -1;
            return 0;
        }
        else if(strncmp("SERV",readBuf,4) == 0){
            status = sscanf(readBuf, "%s %d",msgType, &balls_left);
            if (status != 2 || status == EOF)
                return -1;
            return SERV;
        }
    }
    //wait state
    else if(*gs == WAIT){
        if(strncmp("BALL",readBuf,4) == 0){
            int netPos, xttm, yttm, ydir;
            char sym;
            status = sscanf(readBuf, "%s %d %d %d %d %c\r\n",
	                msgType, &netPos,&xttm,&yttm, &ydir,&sym );
            if(status != 6 || status == EOF)
                return -1;
            
            ball->y_pos = TOP_ROW + netPos;
            ball->x_ttm = ball->x_ttg = xttm;
            ball->y_ttm = ball->y_ttg = yttm;
            ball->y_dir = ydir;
            ball->symbol = sym;
            return BALL;
        }
        else if(strncmp("MISS",readBuf,4) == 0){
            return MISS;
        }
        else if(strncmp("DONE",readBuf,4) == 0){
            return DONE;
        }
    }
   
}


void unknownMsg(){
    printf("unknown message received");
}
