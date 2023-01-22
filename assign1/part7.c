#include <stdio.h>

int main(){
    printf("enter text to be counted, followed by a \'.\':\n");
    int charNum=0,
        wordNum=0,
        lineNum=0;
    char c;
    while((c = getchar()) != '.'){
        if(c == ' ') ++wordNum;
        else if(c == '\n'){
             ++lineNum;
             ++wordNum;
        }
        else ++charNum;
    }
    //accounts for the last word and line which don't have a space or /n after.
    ++wordNum;
    ++lineNum;
    printf("chars: %i\nwords: %i\nlines: %i\n",charNum,wordNum,lineNum);
}
