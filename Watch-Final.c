// CS4-53203: Systems Programing
// Name: Dakota Sicher
// Date: 03 Feb 2023
// Watch-Final.c
//Requires utemplib.c to compile
#include    <stdio.h>
#include    <unistd.h>
#include    <utmp.h>
#include    <fcntl.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <signal.h>

static int userCnt;
static int waitTime = 300;
static char** watchList;

void readUsers(int*);
void swapStatus(int**, int**);
void printLogs(int*, int*);

int main(int ac, char* av[])
{
    //ensures default action(terminate) on hangup
    signal(SIGHUP, SIG_DFL); 

    if(ac == 1){
        printf("Enter a list of user names to watch.\n");
        printf("If an interval other than 5 min is desired,");
        printf(" specify as first argument(seconds).\n");
        exit(0);
    }

    //check if first arg is all numbers ie a user specified interval
    size_t len = strlen(av[1]);
    int userInter = 1;
    for(int i=0; i<len; ++i){
        if(!isdigit(av[1][i])) userInter = 0;
    }

    //userInter == 1 if all chars in av[1] are digits

    //set userCnt and start of watchlist based on
    //if an interval was entered.
    userCnt = ac-userInter-1;
    watchList = &av[1+userInter];
    if(userInter== 1){
        waitTime = atoi(av[1]);
    }
    if(userCnt < 1){
        printf("No users specified.\n");
        exit(0);
    }

    //using an int array to compare user status at previous check
    int* prevStatus = calloc(userCnt,sizeof(int));
    int* newStatus = calloc(userCnt,sizeof(int));

    //read utmp for initial user status
    readUsers(newStatus);
    for(int i = 0; i < userCnt; ++i){
        if(newStatus[i] == 1)
            printf("%s ",watchList[i]);
    }
    printf("are currently logged in.\n");
    swapStatus(&prevStatus, &newStatus); // prevStatus gets current status for next check

    //start watching
    while(1){
//        printf("...\n");
        sleep(waitTime);
        readUsers(newStatus);
        printLogs(prevStatus,newStatus);
        swapStatus(&prevStatus,&newStatus);
    }

    free(prevStatus); free(newStatus);
    return 0;
}

void readUsers( int* currentStatus){

    struct utmp*    utbufp;

    if ( utmp_open(UTMP_FILE) == -1 ){
        perror(UTMP_FILE);
        exit(1);
    }
    //zero out current status
    for(int i=0; i < userCnt; ++i)
        currentStatus[i] = 0;

/*
read each utmp record and check if the ut.user is one of the watch list
if it is set status to 1.
*/
    while( (utbufp = utmp_next()) != ((struct utmp*)NULL) ){
        for(int i = 0; i < userCnt;++i){
            if(strcmp(utbufp->ut_user, watchList[i]) == 0)
                currentStatus[i] = 1;
        }
    }
    utmp_close();
}

//swaping current and previous status make updating previous each round easy
//current get wiped each round anyway
void swapStatus(int**  prev, int** cur){
    int* tmp = *prev;
    *prev = *cur;
    *cur = tmp;
}

void printLogs(int* prev, int* cur){
    int setPrint=0;

//print loggins
    for(int i = 0; i < userCnt; ++i){
        if(prev[i] == 0 && cur[i] == 1){
            printf("%s ", watchList[i]);
            setPrint = 1;
        }
    }
    if(setPrint == 1) printf("logged in.\n");

//print logouts
    setPrint = 0;
    for(int i = 0; i < userCnt; ++i){
        if(prev[i] == 1 && cur[i] == 0){
            printf("%s ", watchList[i]);
            setPrint = 1;
        }
    }
    if(setPrint == 1) printf("logged out.\n");
}

