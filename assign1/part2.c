#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]){

    FILE *fp;
    char* fileName = argv[1];
    char* searchStr = argv[2];
    char buff[255];


    fp = fopen(fileName,"r");
    //printf("filename is %s.\n",fileName);
    //printf("word to search is %s.\n",word);

    while( (fscanf(fp,"%s",buff) == 1) ){
        //printf("read from the file: %s\n",buff);
        if( memcmp(buff,searchStr,strlen(searchStr)) == 0){
            printf("found: %s\n", buff);
        }
    }

    fclose(fp);
}
