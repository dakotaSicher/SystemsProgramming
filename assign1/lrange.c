#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if(argc != 4){
        printf("wrong number of arguments");
        exit(1);
    }
    FILE *fd;
    fd = fopen(argv[3],"r");
    int llim = atoi(argv[1]);
    int ulim = atoi(argv[2]);

    char *line = NULL;
    size_t len = 0;
//    fpos_t position;

    for(int i = 1; i < llim; ++i){
        getline(&line,&len,fd);
        if(feof(fd)) break;
    }

    for(int i = llim; i <= ulim; ++i){
        getline(&line,&len,fd);
        if(feof(fd)) break;
        printf("%s", line);
    }
    fclose(fd);
}
