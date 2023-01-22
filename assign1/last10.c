#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE *fd;
    if(argc == 2){
        fd = fopen(argv[1],"r");
    }
    else{
        fd = fopen("tmp.txt","w+");
        char c;
        while((c=getchar()) != EOF){
            fputc(c,fd);
        }
        rewind(fd);
    }
    char* line = NULL;
    size_t len = 0;


    //find position of (n-10)th line
    int lineNum = 0;
    while(getline(&line,&len,fd)!= -1){
        ++lineNum;
    }
    rewind(fd);

    for(int i=1;i<=lineNum;++i){
        getline(&line,&len,fd);
        if(i >= (lineNum-9))
            printf("%s",line);
    }

    fclose(fd);
}
