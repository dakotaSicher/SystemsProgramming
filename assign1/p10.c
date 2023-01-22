#include <stdio.h>
#include <stdlib.h>

struct numlist{
    float*  list;
    int     len;
    float   min,
            max,
            avg;
};

void compute_stats(struct numlist *listptr);

int main(){ 
    struct numlist *list1;
    list1 = malloc(sizeof(struct numlist));
    list1->len = 5;
    list1->list = malloc(list1->len*sizeof(float));
    for(int i = 0; i < list1->len; ++i){
        list1->list[i] = i + .1;
    }
    compute_stats(list1);
    printf("min: %f\n",list1->min);
    printf("max: %f\n",list1->max);
    printf("avg: %f\n",list1->avg);
}

void compute_stats(struct numlist *listptr){
    float total=0;
    if(listptr->len == 0) return;
    listptr->min = listptr->list[0];
    listptr->max = listptr->list[0];
    for(int i = 0; i < listptr->len; ++i){
        total += listptr->list[i];
        if(listptr->list[i] < listptr->min) listptr->min = listptr->list[i];
        if(listptr->list[i] > listptr->max) listptr->max = listptr->list[i];
    }
    listptr->avg = total/listptr->len;
}

