// CS4-53203: Systems Programming
// Name:
// Date:
// SicherDakota-sttyl.c-final.txt

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

void print_sttyl(struct termios* state);
void set_kill(struct termios* state,char* killKey);
void set_erase(struct termios* state,char* eraseKey);
int  sel_attr(char* attr);
void set_attr(struct termios* state, char* attr);


int main(int ac, char* av[]){

    struct  termios ttyState;
    if ( tcgetattr( 0 , &ttyState ) == -1 ){
        perror( "cannot access tty");
        exit(1);
    }

    if(ac == 1)
        print_sttyl(&ttyState);

    while(--ac){
        ++av;
        if(strcmp(*av,"kill")==0){
            printf("setting kill key\n");
            set_kill(&ttyState,*++av);
            --ac;
        }
        else if(strcmp(*av,"erase")==0){
            set_erase(&ttyState,*++av);
            --ac;
        }
        else set_attr(&ttyState,*av);
    }

    if ( tcsetattr(0,TCSANOW, &ttyState) == -1)
        printf("Unable to save attributes\n");
}

struct flagset {int flagValue; char* string;};

struct flagset attr_values[] = {
    ICRNL ,"icrnl",
    ONLCR ,"onlcr",
    OLCUC ,"olcuc",
    TAB3  ,"tabs",
    ECHO  ,"echo",
    ECHOE ,"echoe",
    ICANON,"icanon",
    ISIG  ,"isig"};

void print_sttyl(struct termios* state){
    printf("intr: ^%-4c",state->c_cc[VINTR]-1+'A');
    printf("kill: ^%-4c",state->c_cc[VKILL]-1+'A');
    if(state->c_cc[VERASE] == 127)
        printf("erase: ^?   ");
    else
        printf("erase: ^%-4c",state->c_cc[VERASE]-1+'A');
    putchar('\n');

    if(!(state->c_iflag & ICRNL)) putchar('-');
    printf("icrnl ");

    for(int i=1; i<3;++i){
        if(!(state->c_oflag & attr_values[i].flagValue)) putchar('-');
        printf("%s ",attr_values[i].string);
    }

    if(state->c_oflag & TAB3) putchar('-');
    printf("tabs ");

    for(int i=4; i<8;++i){
        if(!(state->c_lflag & attr_values[i].flagValue)) putchar('-');
        printf("%s ",attr_values[i].string);
    }
    putchar('\n');
}
void set_kill(struct termios* state, char* killKey){
    printf("inside set_kill\n");
    char k;
    if(killKey[0] == '^'){
        k = killKey[1];
        k = k - 'A' + 1;
    }
    else k = killKey[0];
    state->c_cc[VKILL] = k;
}
void set_erase(struct termios* state,char* eraseKey){
    char e;
    if(eraseKey[0] == '^'){
        if(eraseKey[1] == '?'){
            e = 127;
        }
        else{
            e = eraseKey[1];
            e = e - 'A' + 1;
        }
    }
    else e = eraseKey[0];
    state->c_cc[VERASE] = e;
}


//converts acceptable attriblute arguments into case number for
//set_attr switch statement
int sel_attr(char* attr){
    for(int i = 0; i<8;++i)
        if(strcmp(attr, attr_values[i].string)==0) {
            printf("allowed attr\n");
            return i+1;
        }
}

void set_attr(struct termios* state, char* attr){

    int setOff = 0;
    if(attr[0] == '-')
        setOff = 1;
//    printf("attr+setOff = %s", attr+setOff);
    int c = sel_attr(attr+setOff);
    printf("sel_attr returns: %d\n", c);
    switch(c){
        case 1:
            if(setOff) state->c_iflag &= ~ICRNL;
            else state->c_iflag |= ICRNL;
            break;
        case 2:
            if(setOff) state->c_oflag &= ~ONLCR;
            else state->c_oflag |= ONLCR;
            break;
        case 3:
            if(setOff) state->c_oflag &= ~OLCUC;
            else state->c_oflag |= OLCUC;
            break;
        case 4:
            if(setOff) state->c_oflag |= TAB3;
            else state->c_oflag &= ~TAB3;
            break;
        case 5:
            printf("case echo\n");
            if(setOff) state->c_lflag &= ~ECHO;
            else state->c_lflag |= ECHO;
            break;
        case 6:
            if(setOff) state->c_lflag &= ~ECHOE;
            else state->c_lflag |= ECHOE;
            break;
        case 7:
            if(setOff) state->c_lflag &= ~ICANON;
            else state->c_lflag |= ICANON;
            break;
        case 8:
            if(setOff) state->c_lflag &= ~ISIG;
            else state->c_lflag |= ISIG;
            break;
        default:
            printf("%s: unknown mode\n", attr);
    }
}
