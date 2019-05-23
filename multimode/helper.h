#ifndef HELPER_H
#define HELPER_H

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

typedef void (*funcp)(void* arg);

struct thread_info{
    struct task_struct *tstruct;
    int* input;
    int* running;
};

struct task_struct{
    int* priority;
    struct timespec* period;
    struct timespec begin;
    int* limit;
    funcp* function;
};

void printTimespec(const char* str, struct timespec t){
    printf("%s", str);
    printf("%li : %li\n", t.tv_sec, t.tv_nsec);
}

struct timespec intNsToTime(int time){
    struct timespec ret;
    ret.tv_nsec = time % 1000000000;
    ret.tv_sec = time / 1000000000;

    return ret;
}

int timeToIntNs(struct timespec time){
    int ns = time.tv_sec * 1000000000;
    ns += time.tv_nsec;
    return ns;
}

void initTaskStruct(struct task_struct* tstruct, int taskCount, int modes){
    for(int i = 0; i < taskCount; i++){
        tstruct[i].priority = (int*) malloc(modes*sizeof(int));
        tstruct[i].period = (struct timespec*) malloc(modes*sizeof(struct timespec));
        tstruct[i].limit = (int*) malloc(modes*sizeof(int));
        tstruct[i].function = malloc(modes*sizeof(funcp));
    }
}

void freeTaskStruct(struct task_struct* tstruct, int taskCount){
    for (int i = 0; i < taskCount; i++){
        free(tstruct[i].priority);
        free(tstruct[i].period);
        free(tstruct[i].limit);
        free(tstruct[i].function);
    }
}

void printTaskStruct(struct task_struct* m, int modes){
    for (int i = 0; i < modes; i++){
        printf("Modde: %i \n" , i);
        printf("Priority: %i \n", m->priority[i]); 
        printf("Limit: %i \n", m->limit[i]); 
        printTimespec("Period: ", m->period[i]);
        printTimespec("Begin: ", m->begin);
        printf("\n");
    }
}

void printTasks(struct task_struct* t, int taskCount, int modes){
    for (int i = 0; i < taskCount; i++){
        printTaskStruct(&t[i], modes);
    }
}

int getRand(int min, int max){
    return (rand () % ((max + 1) - min)) + min;
}

struct timespec diff(struct timespec start, struct timespec end){
    struct timespec tmp;
    tmp.tv_sec = end.tv_sec-start.tv_sec;
    tmp.tv_nsec = end.tv_nsec-start.tv_nsec;

    if (tmp.tv_nsec < 0){
        tmp.tv_nsec += 1000000000;
        tmp.tv_sec -= 1;
    }
    return tmp;
}

struct timespec add(struct timespec t1, struct timespec t2){
    struct timespec tmp;
    tmp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    tmp.tv_sec = t1.tv_sec + t2.tv_sec;

    if (tmp.tv_nsec > 1000000000){
        tmp.tv_nsec -= 1000000000;
        tmp.tv_sec += 1;
    }
    return tmp;
}

void fillThreadInfos(struct thread_info* tinfo, struct task_struct* tstruct, int taskCount, int* run, int* externalInput){   
    for (int i = 0; i < taskCount; i++){
        tinfo[i].input = externalInput;
        tinfo[i].running = run;
        tinfo[i].tstruct = &tstruct[i]; 
    }
}

#endif
